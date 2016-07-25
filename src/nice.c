#include "lib/common.h"

int main(int argc, char *argv[]) {
  options("#n|", .argleast = 1);
  int64_t niceness = flag('n') ? lastnum('n') :
                     flag('#') ? lastnum('#') : 10;
  nice(niceness);
  if (!errno) execvp(argv[1], argv+1);
  return errno;
}
