#include "lib/common.h"

int main(int argc, char *argv[]) {
  options("");
  char *buf;
  size_t size;
  FILE *file = open_memstream(&buf, &size);
  if (argc == 1) *argv-- = "y";
  fputs(*++argv, file);
  while (*++argv) fprintf(file, " %s", *argv);
  fputc('\n', file);
  fflush(file);
  if (size < BUFSIZ / 2) {
    char *tmp = strdupa(buf); // glibc can reuse buf but musl can't.......
    for (size_t i = 1; i * size < BUFSIZ; i++) fputs(tmp, file);
  }
  fclose(file);
  while (1) fputs(buf, stdout);
}
