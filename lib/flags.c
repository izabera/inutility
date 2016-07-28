#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include "common.h"

#define argpush(o, arg, target) do {                                                    \
  flags[opt(o)].count++;                                                                \
  if (!(flags[opt(o)].target =                                                          \
        realloc(flags[opt(o)].target, sizeof(arg) * flags[opt(o)].count))) return '@';  \
  flags[opt(o)].target[flags[opt(o)].count-1] = arg;                                    \
  putc_unlocked(o, optfile);                                                            \
} while (0)

int parseoptind, parseopterr = 1;
struct flags flags[64];
char *flaglist; // flags in the order we got them for sed -e s/a/1/ -f <(echo s/a/2/) -e s/a/3/ <<< aaa

static inline void closefile(FILE **fp) { if (*fp) fclose(*fp); }

int parseopts(int argc, char *argv[], const char *program, struct opts options) {
  char *valid = ":|*?ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789#_",
       /* : == needs string arg
        * | == needs num arg
        * * == needs byte arg
        * # == -num for things like head
        * _ == stop at first negative num for things like seq
        * - == reserved for long opts, to be added eventually */
       *opts = options.shortopts;
  if (strlen(opts) != strspn(opts, valid)) return -1;
  valid++;
  if (*opts == ':') {
    opts++;
    parseopterr = 0;
  }
  if (*opts == ':') return -1;
  char *ptr;
  if ((ptr = strchr(program, '_'))) program = ptr + 1;

  int64_t needarg = 0, neednum = 0, needbyt = 0, maybarg = 0, num;
  int i, j;
  for (i = 0; opts[i]; i++)
         if (opts[i+1] == ':') needarg |= 1LL << opt(opts[i]);
    else if (opts[i+1] == '|') neednum |= 1LL << opt(opts[i]);
    else if (opts[i+1] == '*') needbyt |= 1LL << opt(opts[i]);
    else if (opts[i+1] == '?') maybarg |= 1LL << opt(opts[i]);

  size_t flaglen;
  __attribute__((cleanup(closefile))) FILE *optfile = open_memstream(&flaglist, &flaglen);
  char c;
  for (i = 0; i < argc && argv[i][0] == '-' && argv[i][1]; i++) {
    if (argv[i][1] == '-' && !argv[i][2]) {
      i++;
      break;
    }
    if (!strcmp("--version", argv[i])) {
      if (options.version) puts(options.version);
      else printf("inutility v %s built on %s %s\n", VERSION, __DATE__, __TIME__);
      return 'v';
    }
    if (!strcmp("--help", argv[i])) {
      /* autogenerated docs are better than no docs */
      if (options.descr) printf("%s: %s\n", program, options.descr);
      printf("%s ", program);
      if (options.help) puts(options.help);
      else {
        int brkt = 0;
        for (i = 0; opts[i]; i++) {
          if (opts[i] == '_') continue;
          if (opts[i] == '#') {
            printf("%s-num] ", brkt ? "] [" : "[");
            brkt = 0;
          }
          else if (opts[i+1] == '|' || opts[i+1] == '*' || opts[i+1] == ':' || opts[i+1] == '?') {
            printf("%s-%c %s] ", brkt ? "] [" : "[", opts[i],
                opts[i+1] == '|' ? "num"  :
                opts[i+1] == '?' ? "[arg]"  :
                opts[i+1] == '*' ? "byte" : "arg");
            i++;
            brkt = 0;
          }
          else {
            printf("%s%c", brkt ? "" : "[-", opts[i]);
            brkt = 1;
          }
        }
        if (brkt) printf("] ");

        i = 1;
        if (options.argleast > 0)        /* needed args */
          for (; i <= options.argleast; i++)
            printf("arg%d ", i);
        if (options.arglessthan > 0) {   /* less than lessthan args */
          for (; i < options.arglessthan; i++)
            printf("[arg%d] ", i);
        }
        else printf("[arg...]");

        putchar_unlocked('\n');
      }
      return 'h';
    }
    /* parse numeric options like head -6232345 
     * todo: floats? */
    if (strchr(opts, '#') &&
        (num = parsenumb(argv[i]+1+(argv[i][1] == '-'))) != INT64_MIN) {
      argpush('#', num, nums);
      continue;
    }
    // seq -3 10
    else if (strchr(opts, '_')) {
      char *tmp;
      strtod(argv[i], &tmp);
      if (!*tmp) break;
    }
    for (j = 1; (c = argv[i][j]); j++) {
      if (strchr(":#?|", c)) goto unknown; /* these can be in opts */
      if (strchr(opts, c)) {
        if (needarg & 1LL << opt(c)) {
               if (argv[i][j+1]) argpush(c, &argv[i][j+1], args);
          else if (++i < argc)   argpush(c, &argv[i][0], args);
          else goto missing;
          break;
        }
        else if (neednum & 1LL << opt(c)) {
               if (argv[i][j+1]) argpush(c, parsenumb(&argv[i][j+1]), nums);
          else if (++i < argc)   argpush(c, parsenumb(&argv[i][0]), nums);
          else goto missing;
          break;
        }
        else if (needbyt & 1LL << opt(c)) {
               if (argv[i][j+1]) argpush(c, parsebyte(&argv[i][j+1]), nums);
          else if (++i < argc)   argpush(c, parsebyte(&argv[i][0]), nums);
          else goto missing;
          break;
        }
        else if (maybarg & 1LL << opt(c)) {
               if (argv[i][j+1]) argpush(c, &argv[i][j+1], args);
          else if (++i < argc)   argpush(c, &argv[i][0], args);
          else                   argpush(c, NULL, args);
          break;
        }
        else {
          flags[opt(c)].count++;
          putc_unlocked(c, optfile);
        }
      }
      else {
unknown:
        if (parseopterr) fprintf(stderr, "%s: Unknown option: -%c\n", program, c);
        return '?';
missing:
        if (parseopterr) fprintf(stderr, "%s: Missing argument for option: -%c\n", program, c);
        return ':';
      }
    }
  }
  parseoptind = i;
  if (options.argleast > 0 && i+options.argleast > argc) {
    if (parseopterr) fprintf(stderr, "%s: at least %d argument%s\n", program, options.argleast, options.argleast == 1 ? "" : "s");
    return 'n';
  }
  if (options.arglessthan > 0 && i+options.arglessthan <= argc) {

    if (parseopterr) fprintf(stderr, "%s: at most %d argument%s\n", program, options.arglessthan-1, options.arglessthan == 2 ? "" : "s");
    return 'n';
  }
  return 0;
}
