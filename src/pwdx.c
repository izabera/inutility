#include "lib/common.h"

int main(int argc, char *argv[]) {
  options("", .argleast = 1);
  char *proc, dir[PATH_MAX];
  while (*++argv) {
    asprintf(&proc, "/proc/%s/cwd", *argv);
    if (readlink(proc, dir, PATH_MAX-1) != -1)
      printf("%s: %s\n", *argv, dir);
    free(proc);
  }
  return errno;
}
