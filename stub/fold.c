#include "lib/common.h"

int main(int argc, char *argv[]) {
  options("bsw|");
  FILE *fileptr = stdin;
  ssize_t read, ret;
  char *line = NULL, *ptr;
  size_t len = 0, width = flag('w') ? lastnum('w') : 80, i, col;
  if (argc == 1) {
    argv[0] = "-";
    goto inner;
  }
  while (*++argv) {
         if (argv[0][0] == '-' && argv[0][1] == 0) fileptr = stdin;
    else if (!(fileptr = fopen(argv[0], "r"))) continue;
inner:
    while ((read = getline(&line, &len, fileptr)) > 0) {
      ptr = line;
      if (flag('b')) {
        while ((size_t) read > width+1) {
          if ((ret = fwrite_unlocked(ptr, 1, min(width, (size_t) read), stdout)) == -1) goto nextfile;
          putchar_unlocked('\n');
          ptr += ret;
          read -= ret;
        }
        fwrite_unlocked(ptr, 1, read, stdout);
        continue;
      }
      while (1) {
        for (i = col = 0; i < (size_t) read && col < width; i++) {
          switch (ptr[i]) {
            Case '\b': col -= col > 0;
            Case '\t': col += (col + 1) % 8;
            Case '\r': col = 0;
            Default  : col++;
          }
        }
        if (i == (size_t) read) { fwrite_unlocked(ptr, 1, i, stdout); break; }
        if (flag('s')) {
#if 0  // this doesn't work
          size_t savei = i;
          while (i && !isspace(ptr[i])) i--;
          ret = fwrite_unlocked(ptr, 1, (savei == i || i == 0) ? savei : i+1, stdout);
          ptr += ret;
          read -= ret;
          putchar_unlocked('\n');
#endif
        }
        else {
          ret = fwrite_unlocked(ptr, 1, col, stdout);
          putchar_unlocked('\n');
          ptr += ret;
          read -= ret;
        }
      }
    }
nextfile:
    if (fileptr != stdin) fclose(stdin);
  }
  return errno;
}
