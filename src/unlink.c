#include <unistd.h>
#include <errno.h>

#include "lib/flags.h"
int main(int argc, char *argv[]) {
  options("", .argleast = 1);
  if (argc == 1) return 1;
  while (*++argv) unlink(*argv);
  return errno;
}
