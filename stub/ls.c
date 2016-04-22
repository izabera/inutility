#include <stdio.h>
#include <errno.h>

#include "lib/flags.h"

int main(int argc, char *argv[]) {
  options("aAdilR1");
  while (*++argv) {
  }
  return errno;
}
