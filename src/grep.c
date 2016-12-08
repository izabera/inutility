#include "lib/common.h"

static regex_t *regs;
static struct str *patterns = NULL;
static int regmatch(struct str s, size_t i) {
  regmatch_t m;
  if (regexec(regs+i, s.str, 1, &m, 0) != 0) return -1;
  if (flag('x') && (m.rm_so != 0 || (size_t)m.rm_eo != s.len)) return -1;
  return 1;
}
static int strmatch(struct str s, size_t i) {
  if (flag('x') && s.len != patterns[i].len) return -1;
  return strstr(s.str, patterns[i].str) != 0 ? 1 : -1;
}
static int strcasematch(struct str s, size_t i) {
  if (flag('x') && s.len != patterns[i].len) return -1;
  return strcasestr(s.str, patterns[i].str) != 0 ? 1 : -1;
}

int main(int argc, char *argv[]) {
  options("acEFhHinqsvxe:f:"); // -as are ignored
  FILE *fileptr = stdin;
  ssize_t read;
  size_t count, lineno;

  size_t npatterns = 0;
  // strikingly efficient, great engineering
#define push(p, l) do {                                               \
    patterns = realloc(patterns, ++npatterns * sizeof(struct str));   \
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
          if ((read = getline(&tmp, &s, ftmp)) != -1) {
            if (tmp[read-1] == '\n') tmp[--read] = 0;
            push(tmp, read);
          }
          else break;
        } while (1);
      }
    }
    for (size_t i = 0; i < flag('e'); i++)
      push(flags[opt('c')].args[i], strlen(flags[opt('c')].args[i]));
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
            (!!flag('i')*REG_ICASE) | (!!flag('E')*REG_EXTENDED)) != 0)
          return 255;
  }

  int (*matchfunc)(struct str, size_t) =
    flag('F') ? flag('i') ? strcasematch : strmatch : regmatch;

  int matched = 0, flagv = flag('v') ? -1 : 1;
  if (argc == 1) {
    argv[0] = "-";
    goto inner;
  }
  if (argc > 2) flag('H') = 1;
  while (*++argv) {
         if (argv[0][0] == '-' && argv[0][1] == 0) fileptr = stdin;
    else if (!(fileptr = fopen(argv[0], "r"))) continue;
inner:
    count = lineno = 0;
    struct str line;
    size_t tmp;
    while ((read = getline(&line.str, &tmp, fileptr)) > 0) {
      if (line.str[read-1] == '\n') line.str[--read] = 0;
      line.len = read;
      lineno++;
      for (size_t i = 0; i < npatterns; i++)
        if (matchfunc(line, i) * flagv > 0) {
          if (flag('q')) return 0;
          matched = 1;
          if (flag('c')) count++;
          else {
            if (flag('H') && !flag('h')) printf("%s:", *argv);
            if (flag('n')) printf("%zu:", lineno);
            puts(line.str);
          }
        }
    }
    if (flag('c')) printf("%zu\n", count);
    if (fileptr != stdin) fclose(fileptr);
  }
  return (!matched) | errno;
}
