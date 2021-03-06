#include "lib/common.h"

// mostly written by ybden

int main(int argc, char *argv[]) {
  options("acfmr:t:d:", .argleast = 1);
  int fd;
  struct timespec ts[2] = {
    { 0, UTIME_NOW },
    { 0, UTIME_NOW },
  };

  if (flag('d') && lastarg('d')[strlen(lastarg('d'))-1] == 'Z') setenv("TZ", "UTC", 1);
  struct tm *tm = localtime(&(time_t) { time(NULL) });

  if (flag('r')) {
    struct stat st;
    if (stat(lastarg('r'), &st) == -1) return errno;
    ts[0] = st.st_atim;
    ts[1] = st.st_mtim;
  }

  else if (flag('t')) {
    char *seconds, *tmp = lastarg('t');
    if ((seconds = strchr(tmp, '.'))) {
      *seconds++ = 0;
      tm->tm_sec = strtol(seconds, NULL, 10);
    }
    else tm->tm_sec = 0;
    size_t len = strlen(tmp);
    if (len != 8 && len != 10 && len != 12) return -1;
    strptime(tmp, "%C%y%m%d%H%M" + 12 - len, tm);
    ts[0].tv_sec  = ts[1].tv_sec  = mktime(tm);
    ts[0].tv_nsec = ts[1].tv_nsec = 0;
  }

  else if (flag('d')) {
    char *frac, *tmp = strchr(lastarg('d'), ' '), buf[10] = "000000000";
    if (tmp) *tmp = 'T'; // it must be either a space or T
    frac = strptime(lastarg('d'), "%Y-%m-%dT%H:%M:%S", tm);
    ts[0].tv_sec  = ts[1].tv_sec  = mktime(tm);
    ts[0].tv_nsec = ts[1].tv_nsec = 0;
    if (*frac == '.' || *frac == ',') {
      strtoul(++frac, &tmp, 10);
      tmp[0] = 0;
      memmove(buf, frac, min(strlen(frac), 9));
      ts[0].tv_nsec = ts[1].tv_nsec = strtoul(buf, NULL, 10);
    }
  }

  if (flag('m') && !flag('a')) ts[0].tv_nsec = UTIME_OMIT;
  if (!flag('m') && flag('a')) ts[1].tv_nsec = UTIME_OMIT;

  while (*++argv) {
    if (argv[0][0] == '-' && argv[0][1] == 0) {
      futimens(1, ts);
      continue;
    }
    int olderrno = errno;
    if (access(argv[0], F_OK) == -1) {
      errno = olderrno;
      if (flag('c')) continue;
      if ((fd = creat(argv[0], 0644)) != -1) {
        futimens(fd, ts);
        close(fd);
      }
    }
    else
      utimensat(AT_FDCWD, argv[0], ts, 0);
  }
  return errno;
}
