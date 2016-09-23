#include <fcntl.h>
inline void sequential(int fd) {
  /* cannot do both in one call because the api is dumb
   * posix_fadvise(fd, 0, 0, POSIX_FADV_SEQUENTIAL|POSIX_FADV_WILLNEED);
   * only do the sequential bit (at least for now) */
  posix_fadvise(fd, 0, 0, POSIX_FADV_SEQUENTIAL);
}
