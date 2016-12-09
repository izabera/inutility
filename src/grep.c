#include "lib/common.h"

static regex_t *regs;
static struct str *patterns = NULL;

static int checkborder(struct str h, struct str n) {
  if (h.str != n.str) {
    if (n.str[-1] == '_' || isalnum(n.str[-1])) return -1;
  }
  if (n.str[n.len] == '_' || isalnum(n.str[n.len])) return -1; // n.len and not n.len -1
  return 1;
}

static int regmatch(struct str s, size_t i) {
  regmatch_t m;
  if (regexec(regs+i, s.str, 1, &m, 0) != 0) return -1;
  if (flag('x') && (m.rm_so != 0 || (size_t)m.rm_eo != s.len)) return -1;
  struct str tmp = { .str = s.str+m.rm_so, .len = m.rm_eo - m.rm_so };
  return (flag('w')) ? checkborder(s, tmp) : 1;
}

static int strmatch(struct str s, size_t i) {
  if (flag('x') && s.len != patterns[i].len) return -1;
  struct str tmp = { .str = strstr(s.str, patterns[i].str), .len = patterns[i].len };
  if (tmp.str == NULL) return -1;
  return (flag('w')) ? checkborder(s, tmp) : 1;
}

static int matched, flagv;
static int (*matchfunc)(struct str, size_t);
static size_t npatterns;
static int grep(const char *path, struct stat *st, int type, struct FTW *ftw) {
  FILE *fileptr;
  UNUSED(st);
  if (ftw) { // recursive
    if (!(fileptr = fopen(path, "r"))) return 0; // 0 to continue with nftw
    if (type & (FTW_NS|FTW_SL|FTW_SLN|FTW_D|FTW_DP|FTW_DNR)) return 0;
  }
  else {
         if (path[0] == '-' && path[0] == 0) {
           path = "(standard input)"; // posix requires this...
           fileptr = stdin;
         }
    else if (!(fileptr = fopen(path, "r"))) return 0;
  }

  static struct str line = { 0 }; // <- static
  static size_t tmp = 0;          // <- static
  size_t count = 0, lineno = 0;
  ssize_t read;
  while ((read = getline(&line.str, &tmp, fileptr)) > 0) {
    if (line.str[read-1] == '\n') line.str[--read] = 0;
    line.len = read;
    lineno++;
    for (size_t i = 0; i < npatterns; i++)
      if (matchfunc(line, i) * flagv > 0) {
        if (flag('q')) return 0;
        matched = 1;
        count++;
        if (flag('l')) {
          puts(path);
          goto nextfile;
        }
        if (!flag('c') && !flag('L')) {
          if (flag('H') && !flag('h')) printf("%s:", path);
          if (flag('n')) printf("%zu:", lineno);
          puts(line.str);
        }
        break;
      }
  }
  if (!count && flag('L')) {
    puts(path);
    goto nextfile;
  }
  if (flag('c')) printf("%zu\n", count);
nextfile:
  if (fileptr != stdin) fclose(fileptr);
  return 0;
}

int main(int argc, char *argv[]) {
  if (!(strcmp(argv[0], "fgrep"))) flag('F') = 1;
  if (!(strcmp(argv[0], "egrep"))) flag('E') = 1;
  options("acEFGhHilLnqrsvwxe:f:"); // -as are ignored

  // strikingly efficient, great engineering
#define push(p, l) do {                                               \
    patterns = realloc(patterns, ++npatterns * sizeof(struct str));   \
    if (flag('i')) lowerstr(p);                                       \
    patterns[npatterns-1].str = p;                                    \
    patterns[npatterns-1].len = l;                                    \
  } while (0)

  if (flag('e') || flag('f')) {
    if (flag('f')) {
      FILE *ftmp;
      for (size_t i = 0; i < flag('f'); i++) {
        if (!(ftmp = fopen(flags[opt('f')].args[i], "r"))) return errno;
        do {
          char *tmp = NULL;
          size_t s = 0;
          ssize_t read;
          if ((read = getline(&tmp, &s, ftmp)) != -1) {
            if (tmp[read-1] == '\n') tmp[--read] = 0;
            push(tmp, read);
          }
          else break;
        } while (1);
      }
    }
    for (size_t i = 0; i < flag('e'); i++)
      push(flags[opt('e')].args[i], strlen(flags[opt('e')].args[i]));
  }
  else {
    if (argc == 1) return 255;
    push(argv[1], strlen(argv[1]));
    argc--;
    argv++;
  }

  if (!flag('F')) {
    regs = malloc(sizeof(regex_t) * npatterns);
    for (size_t i = 0; i < npatterns; i++)
      if (regcomp(regs+i, patterns[i].str,
            (!!flag('i')*REG_ICASE) | (!!flag('E')*!flag('G')*REG_EXTENDED)) != 0)
          return 255;
  }

  matchfunc = flag('F') ? strmatch : regmatch;

  flagv = flag('v') ? -1 : 1;
  if (flag('r')) {
    flag('H') = 1;
    if (argc == 1) *argv-- = ".";
    while (*++argv)
      nftw(*argv,
          (int (*)(const char *, const struct stat *, int, struct FTW *))grep,
          100, 0);
  }
  else {
    if (argc == 1) *argv-- = "-";
    if (argc > 2) flag('H') = 1;
    while (*++argv) grep(*argv, &(struct stat){ 0 }, 0, 0);
  }
  return (!matched) | errno;
}
