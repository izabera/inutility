#include "lib/common.h"

#define fail(...) do { fprintf(stderr, __VA_ARGS__); unportable = 1; goto nextloop; } while (0)
int main(int argc, char *argv[]) {
  options("pP", .argleast = 1);
  int unportable = 0;
  size_t pathlen = flag('p') ? _POSIX_PATH_MAX : PATH_MAX,
         namelen = flag('p') ? _POSIX_NAME_MAX : NAME_MAX;
nextloop:
  while (*++argv) {
    if (flag('P')) {
      if (argv[0][0] == 0) fail("empty file name\n");
      if (argv[0][0] == '-' || strstr(*argv, "/-")) fail("leading - in `%s'\n", *argv);
    }
    if (flag('p') /* fingers crossed */ && strlen(*argv) !=
        strspn(*argv, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789._-/"))
      fail("`%s' contains unportable characters\n", *argv);
    if (strlen(*argv) >= pathlen) fail("`%s' longer than %zu bytes\n", *argv, pathlen);

    struct stat st;
    if (lstat(*argv, &st) == -1 && errno != ENOENT) fail("can't stat `%s'\n", *argv);

    for (char *p = strtok(*argv, "/"); p; p = strtok(NULL, "/"))
      if (strlen(p) > namelen)
        fail("`%s' has a component longer than %zu bytes\n", *argv, namelen);
  }
  return unportable; // this doesn't return errno because only lstat can set it
}
