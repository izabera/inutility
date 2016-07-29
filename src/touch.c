#include "lib/common.h"

int main(int argc, char *argv[]) {
  options("acfmr:t:d:", .argleast = 1); // todo -t -d
  int fd;
  struct timespec ts[2] = {
    { 0, UTIME_NOW },
    { 0, UTIME_NOW },
  };
  if (flag('r')) {
    struct stat st;
    if (stat(lastarg('r'), &st) == -1) return errno;
    ts[0].tv_sec  = ts[1].tv_sec  = st.st_mtim.tv_sec;
    ts[0].tv_nsec = ts[1].tv_nsec = st.st_mtim.tv_nsec;
  }
  else if (flag('t')) {
    char *informat, *seconds, *tmp = lastarg('t');
    struct tm *tm = localtime(&(time_t) { time(NULL) });
    if ((seconds = strchr(tmp, '.'))) {
      *seconds++ = 0;
      tm->tm_sec = strtol(seconds, NULL, 10);
    }
    switch (strlen(tmp)) {
      Case  8: informat = "%m%d%H%M"    ;
      Case 10: informat = "%y%m%d%H%M"  ;
      Case 12: informat = "%C%y%m%d%H%M";
      Default: return -1;
    }
    strptime(tmp, informat, tm);
    ts[0].tv_sec  = ts[1].tv_sec  = mktime(tm);
    ts[0].tv_nsec = ts[1].tv_nsec = 0;
  }
  else if (flag('d')) {
    struct tm tm;
    char *frac = strptime(lastarg('d'), "%Y-%m-%d %H:%M:%S", &tm), *tmp, buf[10] = "000000000";
    if (*frac == '.' || *frac == ',') {
      strtoul(++frac, &tmp, 10);
      if (*tmp == 'Z') setenv("TZ", "UTC", 1);
      tmp[0] = 0;
      memmove(buf, frac, min(strlen(frac), 9));
      ts[0].tv_sec  = ts[1].tv_sec  = mktime(&tm);
      ts[0].tv_nsec = ts[1].tv_nsec = strtoul(buf, NULL, 10);
    }
  }
  if (flag('m') && !flag('a')) ts[0].tv_nsec = UTIME_OMIT;
  if (!flag('m') && flag('a')) ts[1].tv_nsec = UTIME_OMIT;
  while (*++argv) {
    if (argv[0][0] == '-' && argv[0][1] == 0) {
      futimens(0, ts);
      continue;
    }
    int olderrno = errno;
    if (access(argv[0], F_OK) == -1) {
      errno = olderrno;
      if (flag('c')) continue;
      if ((fd = creat(argv[0], 0666)) != -1) {
        futimens(fd, ts);
        close(fd);
      }
    }
    else
      utimensat(AT_FDCWD, argv[0], ts, 0);
  }
  return errno;
}
