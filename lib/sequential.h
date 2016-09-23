#include <fcntl.h>
inline void sequential(int fd) { posix_fadvise(fd, 0, 0, POSIX_FADV_SEQUENTIAL|POSIX_FADV_WILLNEED); }
