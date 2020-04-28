#include "lib/common.h"

int main(int argc, char *argv[]) {
  options("n");
  if (argc > 1)
    for (int i = 1; i < argc; i++)
      printf("%s%s", i > 1 ? " " : "", argv[i]);
  if (!flag('n')) putchar('\n');
  return errno;
}
