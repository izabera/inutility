#include "lib/common.h"

int main(int argc, char *argv[]) {
  options("abcEFihHlLnoqrsvwxZze:f:m|"); // -as are ignored
  FILE *fileptr = stdin;
  ssize_t read;
  size_t len, count;
  char *line, *match;

  // this is dumb but why not
  char *patterns, *tmp;
  size_t lpatterns;
  FILE *fpatterns = open_memstream(&patterns, &lpatterns), *ftmp;
  if (flag('f')) {
    for (size_t i = 0; i < flag(x); i++) {
      if (!(ftmp = fopen(flags[opt('f')].args[i], "r"))) continue;

    while (flags[opt(x)].args[flag(x)-1]) { }

  int idelim = !flag('z') * '\n', odelim = !flag('Z') * '\n', matched = -1;
  if (argc == 1) {
    argv[0] = "-";
    goto inner;
  }
  while (*++argv) {
         if (argv[0][0] == '-' && argv[0][1] == 0) fileptr = stdin;
    else if (!(fileptr = fopen(argv[0], "r"))) continue;
inner:
    count = 0;
    while ((read = getdelim(&line, &len, idelim, fileptr)) > 0) {
      if (flag('F')) {
        for (size_t q = 0; q =
        if (match = memmem(line, len, 
      }
      else if (flag('E')) {
      }
      else {
      }
    }
    if (flag('c')) printf("%zu", count);
    if (fileptr != stdin) fclose(fileptr);
  }
  return matched | errno;
}
