#include "lib/common.h"

static void parsedate(const char *d, char **informat, struct tm **tm) {
  if (*d == '@') *tm = localtime(&(time_t){ strtol(d+1, 0, 10) });
  else {
    if (flag('D')) *informat = lastarg('D');
    else
      switch (strlen(d)) {
        case  8: *informat = "%m%d%H%M"    ; break;
        case 10: *informat = "%m%d%H%M%y"  ; break;
        case 12: *informat = "%m%d%H%M%C%y"; break;
        default: exit(-1);
      }
    strptime(d, *informat, *tm);
    *tm = localtime(&(time_t) { mktime(*tm) });
  }
}

int main(int argc, char *argv[]) {
  options("RuD:d:r:", .arglessthan = 2); // D like toybox and busybox
  char *informat, *outformat = flag('R') ? "%a, %d %b %Y %T %z" : "%a %b %e %H:%M:%S %Z %Y", outbuf[4096];
  if (flag('u')) setenv("TZ", "UTC", 1);
  struct tm *tm = localtime(&(time_t) { time(NULL) });
  if (flag('d')) parsedate(lastarg('d'), &informat, &tm);
  else if (flag('r')) {
    struct stat st;
    if (stat(lastarg('r'), &st) == -1) return errno;
    tm = localtime(&st.st_mtim.tv_sec);
  }
  if (argc == 1) goto dodate;
  if (argc > 1 && argv[1][0] == '+') {
    outformat = &argv[1][1];
dodate:
    strftime(outbuf, sizeof(outbuf), outformat, tm);
  }
  else {
    parsedate(argv[1], &informat, &tm);
    clock_settime(CLOCK_REALTIME, &(struct timespec) { mktime(tm), 0 });
    strftime(outbuf, sizeof(outbuf), outformat, localtime(&(time_t) { time(NULL) }));
  }
  puts(outbuf);
  return errno;
}
