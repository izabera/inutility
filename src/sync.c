#include "lib/common.h"

int main(int argc, char *argv[]) {
  options("df");
  if (flag('d') && (argc == 1 || flag('f'))) return -1;
  int file = 0;
  int (*func)(int) = flag('d') ? fdatasync : flag('f') ? syncfs : fsync;
  if (argc == 1) sync();
  else
  while (*++argv) {
         if (argv[0][0] == '-' && argv[0][1] == 0) func(0);
    else if ((file = open(argv[0], O_RDONLY)) != -1) { func(file); close(file); }
  }
  return errno;
}
