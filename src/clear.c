#include "lib/common.h"

int main(int argc, char *argv[]) {
  options("", .arglessthan = 1);
  printf("\033[2J\033[H");
  return errno;
}
