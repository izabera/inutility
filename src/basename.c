#include "lib/common.h"

int main(int argc, char *argv[]) {
  options("as:z", .argleast = 1);
  char *suffix = NULL, *ptr;
  size_t baselen, sufflen = 0;
  if (flag('s')) {
    flag('a') = 1;
    suffix = lastarg('s');
  }
  if (!flag('a')) {
    suffix = argv[2];
    argv[2] = NULL;
  }
  if (suffix) sufflen = strlen(suffix);
  while (*++argv) {
    *argv = basename(*argv);
    if (suffix) {
      baselen = strlen(*argv);
      ptr = *argv + baselen - sufflen;
      if (baselen > sufflen && !strcmp(ptr, suffix)) *ptr = 0;
    }
    printf("%s%c", *argv, !flag('z') * '\n');
  }
  return errno;
}
