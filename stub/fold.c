#include "lib/common.h"

int main(int argc, char *argv[]) {
  options("bsw|");
  FILE *fileptr = stdin;
  ssize_t read;
  size_t len, c;
  char *line, *tmp, c;
  if (!flag('w')) flag('w') = 80;
  if (argc == 1) {
    argv[0] = "-";
    goto inner;
  }
  while (*++argv) {
         if (argv[0][0] == '-' && argv[0][1] == 0) fileptr = stdin;
    else if (!(fileptr = fopen(argv[0], "r"))) continue;
inner:
    while ((read = getline(&line, &len, fileptr)) > 0) {
      tmp = line;
      if ((c = flag('w')))
        for (c = flag('w'); c && (line[c] == '\t' || line[c] == ' '; c--)
        if (line[flag('w'
        line[flag('w')] = 0;
        

      }
    }
    if (fileptr != stdin) fclose(stdin);
  }
  return errno;
}
