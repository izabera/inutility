#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>

#include <stdio.h>
#include "lib/flags.h"

int main(int argc, char *argv[]) {
  options("", .help = "number[suffix]");
  if (argc == 1) return 1;
  argv++;
  double arg = strtod(*argv, argv);
  if (arg < 0) return 1;
  switch (**argv) {
    case 's': case 0:       break;
    case 'm': arg *=    60; break;
    case 'h': arg *=  3600; break;
    case 'd': arg *= 86400; break;
    default: return 1;
  }
  struct timespec tv = { arg, 0 };
  tv.tv_nsec = (arg - tv.tv_sec) * 1000000000;
  nanosleep(&tv, NULL);

  return errno;
}
