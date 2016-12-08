#include "lib/common.h"

static regex_t *regs;
static char **patterns = NULL;
static int regmatch(const char *str, size_t i) {
  return regexec(regs+i, str, 0, NULL, 0) == 0 ? 1 : -1;
}
static int strmatch(const char *str, size_t i) {
  return strstr(str, patterns[i]) != 0 ? 1 : -1;
}
static int strcasematch(const char *str, size_t i) {
  return strcasestr(str, patterns[i]) != 0 ? 1 : -1;
}

int main(int argc, char *argv[]) {
  options("ciEFqsve:f:"); // -as are ignored
  FILE *fileptr = stdin;
  ssize_t read;
  size_t len, count;
  char *line;

  size_t npatterns = 0;
  // strikingly efficient, great engineering
#define push(p) do {                                            \
    patterns = realloc(patterns, ++npatterns * sizeof(char*));  \
    patterns[npatterns-1] = p;                                  \
  } while (0)

  if (flag('e') || flag('f')) {
    if (flag('f')) {
      FILE *ftmp;
      for (size_t i = 0; i < flag('f'); i++) {
        if (!(ftmp = fopen(flags[opt('f')].args[i], "r"))) return errno;
        do {
          char *tmp = NULL;
          size_t s = 0;
          if (getline(&tmp, &s, ftmp) != -1) {
            if (tmp[s-1] == '\n') tmp[s-1] = 0;
            push(tmp);
          }
          else break;
        } while (1);
      }
    }
    for (size_t i = 0; i < flag('e'); i++) push(flags[opt('c')].args[i]);
  }
  else {
    if (argc == 1) return 255;
    push(argv[1]);
    argc--;
    argv++;
  }

  if (!flag('F')) {
    regs = malloc(sizeof(regex_t) * npatterns);
    for (size_t i = 0; i < npatterns; i++)
      if (regcomp(regs+i, patterns[i],
            REG_NOSUB | (!!flag('i')*REG_ICASE) | (!!flag('E')*REG_EXTENDED)) != 0)
          return 255;
  }

  int (*matchfunc)(const char *, size_t) =
    flag('F') ? flag('i') ? strcasematch : strmatch : regmatch;

  int matched = 0, flagv = flag('v') ? -1 : 1;
  if (argc == 1) {
    argv[0] = "-";
    goto inner;
  }
  while (*++argv) {
         if (argv[0][0] == '-' && argv[0][1] == 0) fileptr = stdin;
    else if (!(fileptr = fopen(argv[0], "r"))) continue;
inner:
    count = 0;
    while ((read = getline(&line, &len, fileptr)) > 0) {
      for (size_t i = 0; i < npatterns; i++)
        if (matchfunc(line, i) * flagv > 0) {
          if (flag('q')) return 0;
          matched = 1;
          if (flag('c')) count++;
          else fputs_unlocked(line, stdout);
        }
    }
    if (flag('c')) printf("%zu\n", count);
    if (fileptr != stdin) fclose(fileptr);
  }
  return (!matched) | errno;
}
