#include "lib/common.h"

int main(int argc, char **argv) {
  options("#n|c*qvz"); /* todo: negative args */
  int64_t number = flag('n') ? flags[opt('n')].nums[flag('n')-1] :
               flag('#') ? flags[opt('#')].nums[flag('#')-1] :
               flag('c') ? flags[opt('c')].nums[flag('c')-1] : 10;

  FILE *fileptr = stdin;
  int firstfile = 0, verbose = flag('v') || (argc > 2 && !flag('q'));
  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  if (argc == 1) {
    argv[0] = "-";
    goto inner;
  }
  while (*++argv) {
         if (argv[0][0] == '-' && argv[0][1] == 0) fileptr = stdin;
    else if (!(fileptr = fopen(argv[0], "r"))) continue;
inner:
    if (verbose) printf("%s==> %s <==\n", firstfile++ ? "\n" : "", argv[0]);
    if (flag('c')) {
      char buf[BUFSIZ];
      int64_t i = number;
      while (i > BUFSIZ) {
        if ((read = fread(buf, 1, BUFSIZ, fileptr)) > 0)
          fwrite(buf, 1, read, stdout);
        else break;
        i -= BUFSIZ;
      }
      if ((read = fread(buf, 1, i, fileptr)) > 0)
        fwrite(buf, 1, read, stdout);
    }
    else {
      for (int64_t i = 0; i < number; i++) {
        if ((read = getdelim(&line, &len, !flag('z') * '\n', fileptr)) > 0)
          fwrite(line, 1, read, stdout);
        else break;
      }
    }
    if (fileptr != stdin) fclose(fileptr);
  }
  return errno;
}
