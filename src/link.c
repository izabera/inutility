#include <unistd.h>
#include <errno.h>

#include "lib/flags.h"
int main(int argc, char *argv[]) {
  options("", .argleast = 2, .arglessthan = 3);
  if (argc < 3) return 1;
  link(argv[1], argv[2]);
  return errno;
}
