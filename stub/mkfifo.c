#include "lib/common.h"

int main(int argc, char *argv[]) {
  options("m:");
  mode_t mode = 0666 & ~getumask();
  if (flag('m')) {
    int64_t tmp = parsemode(lastarg('m'), mode);
    if (tmp == INT64_MIN) return 1;
    mode = tmp;
    printf("%" PRId64 "\n", tmp);
  }
  if (argc == 1) return 1;
  while (*++argv) {
    mkfifo(*argv, mode);
    if (flag('m')) chmod(*argv, mode); // fixes problems with the mask
  }
  return errno;

}
