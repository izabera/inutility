#include "lib/common.h"

int main(int argc, char *argv[]) {
  options("pv", .argleast = 1);
  while (*++argv) {
    do {
      if (flag('v')) printf("removing directory '%s'\n", *argv);
      rmdir(*argv);
      *argv = dirname(*argv);
    } while (flag('p') && strcmp(*argv, ".") && strcmp(*argv, "/"));
  }
  return errno;
}
