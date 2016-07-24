#include "lib/common.h"

int main(int argc, char *argv[]) {
  options("it:");
  FILE *fileptr = stdin;
  if (argc == 1) {
    argv[0] = "-";
    goto inner;
  }
  while (*++argv) {
         if (argv[0][0] == '-' && argv[0][1] == 0) fileptr = stdin;
    else if (!(fileptr = fopen(argv[0], "r"))) continue;
inner: ;
    int c;
    while ((c = getc(fileptr)) != EOF) {
      switch (c) {
        case '\n':
      }
    }
    if (fileptr != stdin) fclose(fileptr);
  }
  return errno;
}
