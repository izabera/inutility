#include "lib/common.h"

#define parsedate(d) do {                                                         \
                       char *_d = d;                                              \
                       if (*_d == '@') tm = func(&(time_t){ atoi(_d+1) });        \
                       else {                                                     \
                         switch (strlen(_d)) {                                    \
                           case  8: informat = "%m%d%H%M"    ; break;             \
                           case 10: informat = "%m%d%H%M%y"  ; break;             \
                           case 12: informat = "%m%d%H%M%C%y"; break;             \
                           default: return -1;                                    \
                         }                                                        \
                         strptime(_d, flag('D') ? lastarg('D') : informat, tm);   \
                         tm = func(&(time_t) { mktime(tm) });                     \
                       }                                                          \
                     } while (0)

int main(int argc, char *argv[]) {
  options("RuD:d:r:", .arglessthan = 2); // D like toybox and busybox
  char *informat, *outformat = flag('R') ? "%a, %d %b %Y %T %z" : "%a %b %e %H:%M:%S %Z %Y", outbuf[4096];
  struct tm tmp, *tm = &tmp;
  struct tm *(*func)(const time_t *) = flag('u') ? gmtime : localtime;
  if (flag('d')) parsedate(lastarg('d'));
  else if (flag('r')) {
    struct stat st;
    if (stat(lastarg('r'), &st) == -1) return errno;
    tm = func(&st.st_mtim.tv_sec);
  }
  else tm = func(&(time_t) { time(NULL) });
  if (argc == 1) goto dodate;
  if (argc > 1 && argv[1][0] == '+') {
    outformat = &argv[1][1];
dodate:
    strftime(outbuf, sizeof(outbuf), outformat, tm);
  }
  else {
    parsedate(argv[1]);
    settimeofday(&(struct timeval) { mktime(tm), 0 }, NULL);
  }
  puts(outbuf);
  return errno;
}
