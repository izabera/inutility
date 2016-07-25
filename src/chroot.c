#include "lib/common.h"

int main(int argc, char *argv[]) {
  options("", .argleast = 1);
  if (chroot(argv[1]) == -1 || chdir("/") == -1) return errno;
  if (argc == 2) execvp("/bin/sh", (char *[]) { "sh", NULL });
  else execvp(argv[2], argv+2);
  return errno;
}
