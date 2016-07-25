#include "lib/common.h"

int main(int argc, char *argv[]) {
  options("s", .arglessthan = 1);
  char *tty = ttyname(0);
  if (!flag('s')) puts(tty ? tty : "not a tty");
  return errno;
}
