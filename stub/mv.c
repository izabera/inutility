#include "lib/common.h"

int main(int argc, char *argv[]) {
  options("finv", .argmin = 2);
  char *target = argv[argc-1];
  argv[argc-1] = NULL;
  struct stat st;
  if (stat(target, &st) == -1) {
    if 
  while (*++argv) {
  }
  return errno;
}
