#include "lib/common.h"

int main(int argc, char *argv[]) {
  options("ls");

  struct { char *str; int n; } errnos[] = {
#include "errnolist.h"
  };

  if (flag('l')) {
    for (size_t i = 0; i < arrsize(errnos); i++)
      printf("%s %d %s\n", errnos[i].str, errnos[i].n, strerror(errnos[i].n));
  }
  else {
    while (*++argv) {
      int num = atoi(*argv), found = 0;
      for (size_t i = 0; i < arrsize(errnos); i++) {
        char *str = strerror(errnos[i].n);
        if ((!strcmp(errnos[i].str, *argv) || errnos[i].n == num) ||
            (flag('s') && strstr(str, *argv))) {
          printf("%s %d %s\n", errnos[i].str, errnos[i].n, strerror(errnos[i].n));
          found = 1;
        }
      }
      if (!found) errno = EINVAL;
    }
  }
  return errno;
}
