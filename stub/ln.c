#include <unistd.h>
#include <errno.h>
#include "lib/flags.h"

int main(int argc, char *argv[]) {
  options("bdfFiLnPrsTvS:t:", .argleast = 1);
  if (flag('F')) flag('d') = 1;
  return errno;
}
