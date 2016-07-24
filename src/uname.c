#include "lib/common.h"

int main(int argc, char *argv[]) {
  options("aimnprsv", .arglessthan = 1);
  struct utsname ubuf;
  if (!flag('m') && !flag('n') && !flag('r') && !flag('v')) flag('s') = 1;
  if (flag('a')) flag('m') = flag('n') = flag('r') = flag('v') = 1;
  if (uname(&ubuf) == -1) return 1;
  printf("%s%s%s%s%s%s%s%s%s%s\n",
    flag('s') ? ubuf.sysname  : "", flag('s') ? " " : "",
    flag('n') ? ubuf.nodename : "", flag('n') ? " " : "",
    flag('r') ? ubuf.release  : "", flag('r') ? " " : "",
    flag('v') ? ubuf.version  : "", flag('v') ? " " : "",
    flag('m') ? ubuf.machine  : "", flag('m') ? " " : "");
  return errno;
}
