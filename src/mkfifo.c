#include "lib/common.h"

int main(int argc, char *argv[]) {
  options("m:");
  mode_t mode = 0777;
  char *end;
  umask(0);
  if (flag('m')) {
    int tmp = strtol(lastarg('m'), &end, 8);
    if (*end) return 1;
    mode = tmp;
  }
  if (argc == 1) return 1;
  while (*++argv) {
    mkfifo(*argv, mode);
  }
  return errno;
}
