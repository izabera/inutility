#include "lib/common.h"

unsigned long col = 0;
static void printchar(char c) {
  putchar_unlocked(c);
  if (++col == flag('w')) {
    putchar_unlocked('\n');
    col = 0;
  }
}

/* translation from https://github.com/izabera/bashutils/blob/master/bash64
 * because why not */
int main(int argc, char *argv[]) {
  options("diw|", .arglessthan = 2);
  FILE *file = stdin;
  if (argc == 2) if (!(file = fopen(argv[1], "r"))) return errno;
  if (flag('w')) flag('w') = lastnum('w');
  else flag('w') = 76;

  char *valid = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";
  char decoded[256] = { 0 };
  int c, count = 0, equal = 0;
  for (char *ptr = valid; *ptr; ptr++) decoded[(int)*ptr] = count++;
  count = 0;
  char old = 0, cur = 0;

  if (flag('d')) {
    while ((c = fgetc_unlocked(file)) != EOF) {
      if (c == '\n') continue;
      if (!decoded[c]) {
        if (!flag('i')) return -1;
        else continue;
      }
      if (c == '=') {
        equal++;
        if (!count) return -1;
      }
      else if (equal > 0) return -1;

      switch (count++) {
        case 0: old = decoded[c]; break;
        case 1: cur = decoded[c]; putchar_unlocked((old<<2)+(cur>>4)); old = cur % 16; break;
        case 2: cur = decoded[c]; putchar_unlocked((old<<4)+(cur>>2)); old = cur %  4; break;
        case 3: cur = decoded[c];
                if (c == '=') equal = 0;
                else putchar_unlocked((old<<6)+cur);
                count = old = 0;
      }
    }
    if (count || old) return -1;
  }
  else {
    while ((c = fgetc_unlocked(file)) != EOF) {
      switch (count++) {
        case 0: printchar(valid[ c>>2 ]);          old = c %  4; break;
        case 1: printchar(valid[(c>>4)+(old<<4)]); old = c % 16; break;
        case 2: printchar(valid[(c>>6)+(old<<2)]); printchar(valid[c%64]); count = 0;
      }
    }
    switch (count) {
      case 1: printchar(valid[old<<4]); printchar('='); printchar('='); break;
      case 2: printchar(valid[old<<2]); printchar('=');
    }
    if (flag('w') && col) putchar_unlocked('\n');
  }
  return errno;
}
