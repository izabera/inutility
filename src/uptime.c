#include "lib/common.h"
#include "lib/uptime.h"

int main(int argc, char *argv[]) {
  options("", .arglessthan = 1);
  return print_uptime();
}
