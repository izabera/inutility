#include "lib/common.h"

int main(int argc, char *argv[]) {
  options("", .argleast = 1);
  while (*++argv) unlink(*argv);
  return errno;
}
