#include "lib/common.h"

int main(int argc, char *argv[]) {
  options("z");
  if (argc == 1) return 1;
  while (*++argv)
    printf("%s%c", dirname(*argv), !flag('z') * '\n');
  return 0;
}
