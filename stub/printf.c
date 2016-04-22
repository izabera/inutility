#include "lib/common.h"

int main(int argc, char *argv[]) {
  options("", .argmin = 1);
  if (argc == 1) return 1;
  char *format = argv[1];
  argv += 2;
  int needarg = 0;
  do {
    for (size_t i = 0; format[i]; i++) {
      switch (format[i]) {
        default: putchar_unlocked(format[i]); break;
        case '%':
          
          switch (format[++i]) {
            case 's':
          }
          break;
      }
    }
  } while (*argv);
  return errno;
}
