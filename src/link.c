#include "lib/common.h"

int main(int argc, char *argv[]) {
  options("", .argleast = 2, .arglessthan = 3);
  link(argv[1], argv[2]);
  return errno;
}
