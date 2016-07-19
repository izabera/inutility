#include "lib/common.h"

int main(int argc, char *argv[]) {
  options("v", .arglessthan = 2);
  if (flag('v') && argc == 1) return -1;
  if (flag('v')) {
  }
  int c;
  while ((c = getchar_unlocked()) != EOF) {
    if (c != '$') putchar_unlocked(c);
    else {
    }
  }
  return errno;
}
