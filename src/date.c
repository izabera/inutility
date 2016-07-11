#include "lib/common.h"

#define parsedate(d) do {                                                       \
                       char *_d = d;                                            \
                       switch (strlen(_d)) {                                    \
                         case  8: informat = "%m%d%H%M"    ; break;             \
                         case 10: informat = "%m%d%H%M%y"  ; break;             \
                         case 12: informat = "%m%d%H%M%C%y"; break;             \
                         default: return -1;                                    \
                       }                                                        \
                       strptime(_d, flag('D') ? lastarg('D') : informat, tm);   \
                     } while (0)

int main(int argc, char *argv[]) {
  options("D:d:r:u"); // D like toybox and busybox
  char *informat = NULL, *outformat = "%a %b %e %H:%M:%S %Z %Y", outdate[4096];
  struct tm tmp, *tm = &tmp;
  if (flag('d')) parsedate(lastarg('d'));
  else tm = localtime(&(time_t) { time(NULL) });
  if (argc == 1) goto dodate;
  if (argc > 1 && argv[1][0] == '+') {
    outformat = &argv[1][1];
dodate:
    strftime(outdate, sizeof(outdate), outformat, tm);
    puts(outdate);
  }
  else {
    parsedate(argv[1]);
    settimeofday(&(struct timeval) { mktime(tm), 0 }, NULL);
  }
  return errno;
}
