#include <unistd.h>
#include <errno.h>
#include "lib/flags.h"

int main(int argc, char *argv[]) {
  options("fsLP");
  return errno;
}
