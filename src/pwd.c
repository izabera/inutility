#include "lib/common.h"

int main(int argc, char *argv[]) {
  options("LP", .arglessthan = 1);
  char *output = NULL, *env = getenv("PWD"), *ptr = env;
  struct stat st1, st2;
  if (!flag('P')) {
    if (env && *env == '/') {
      do {
        if (*ptr++ == '.' && (*ptr == '/' || *ptr == 0)) goto nope;
        if (*ptr == '.' && (ptr[1] == '/' || ptr[1] == 0)) goto nope;
      } while (*ptr);
      if (!stat(env, &st1) && !stat(".", &st2)
          && st1.st_dev == st2.st_dev && st1.st_ino == st2.st_ino)
        output = env;
    }
  }
nope: errno = 0;
  if (output) puts(output);
  else puts(getcwd(NULL, 0)); // glibc and musl and more
  return errno;
}
