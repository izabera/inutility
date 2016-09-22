#include "lib/common.h"

int main(int argc, char *argv[]) {
  options("", .argleast = 1);
  if (signal(SIGHUP, SIG_IGN) != SIG_ERR) {
#define wrap(n, ...) do { int fd = __VA_ARGS__; dup2(fd, n); close(fd); } while (0)
    if (isatty(0)) wrap(0, open("/dev/null", O_RDONLY));
    if (isatty(1)) wrap(1, open("nohup.out", O_APPEND|O_CREAT|O_WRONLY, 0600));
    if (isatty(2)) dup2(1, 2);
    execvp(argv[1], argv+1);
  }
  return 126 + (errno == ENOENT);
}
