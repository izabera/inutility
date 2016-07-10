#include "lib/common.h"

int main(int argc, char *argv[]) {
  options("s", .arglessthan = 1);
  char *tty = ttyname(0);
  if (tty && !flag('s')) puts(tty);
  return !tty;
}
