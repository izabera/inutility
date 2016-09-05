#include "lib/common.h"

int main(int argc, char *argv[]) {
  options("");
  char *buf;
  size_t size;
  FILE *file = open_memstream(&buf, &size);
  if (argc == 1) // optimize for speeeeeeed
    for (size_t i = 0; i < BUFSIZ; i += 2) fputs_unlocked("y\n", file);
  else if (argc == 2) {
    fputs_unlocked(argv[1], file);
    fputc_unlocked('\n', file);
  }
  else {
    fputs_unlocked(*++argv, file);
    while (*++argv) {
      fputc_unlocked(' ', file);
      fputs_unlocked(*argv, file);
    }
    fputc_unlocked('\n', file);
  }
  fclose(file);
  while (1) fputs_unlocked(buf, stdout);
  return 0;
}
