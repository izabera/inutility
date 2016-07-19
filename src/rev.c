#include "lib/common.h"

int main(int argc, char *argv[]) {
  options("");

  int file = 0;
  FILE *fileptr = stdin;
  char *line = NULL, c;
  size_t len = 0, posleft, posright;
  ssize_t read;
  if (argc == 1)
    goto inner;

  while (*++argv) {
         if (argv[0][0] == '-' && argv[0][1] == 0) { file = 0; fileptr = stdin; }
    else if ((file = open(argv[0], O_RDONLY)) == -1) continue;
    else fileptr = fdopen(file, "r");
inner:
      while ((read = getline(&line, &len, fileptr)) != -1) {
        if (read > 1) {
          posleft = 0;
          posright = read -1 - (line[read-1] == '\n');
          while (posright > posleft) {
            c = line[posright];
            line[posright] = line[posleft];
            line[posleft] = c;
            posright--; posleft++;
          }
        }
        fwrite_unlocked(line, 1, read, stdout);
      }
    if (fileptr != stdin) fclose(fileptr);
  }

  return errno;
}
