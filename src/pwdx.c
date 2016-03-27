#include "lib/common.h"

int main(int argc, char *argv[]) {
  options("", .argleast = 1);
  char proc[20], dir[PATH_MAX];
  ssize_t lnk;
  while (*++argv) {
    snprintf(proc, sizeof(proc), "/proc/%ld/cwd", strtol(*argv, NULL, 10));
    if ((lnk = readlink(proc, dir, PATH_MAX-1)) != -1) {
      dir[lnk] = 0;
      printf("%s: %s\n", *argv, dir);
    }
  }
  return errno;
}
