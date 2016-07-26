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
    ts[0].tv_sec  = st.st_atim.tv_sec;
    ts[0].tv_nsec = st.st_atim.tv_nsec;
    ts[1].tv_sec  = st.st_mtim.tv_sec;
    ts[1].tv_nsec = st.st_mtim.tv_nsec;
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
