#include <stdio.h>

#include "lib/flags.h"
int main(int argc, char *argv[]) {
  options("");
       if (argc == 1) while (1) puts("y");
  else if (argc == 2) while (1) puts(argv[1]);

  char *buf;
  size_t size;
  FILE *file = open_memstream(&buf, &size);
  fputs(*++argv, file);
  do { fprintf(file, " %s", *++argv);} while (argv[1]);
  fclose(file);
  while (1) puts(buf);
  return 0;
}
