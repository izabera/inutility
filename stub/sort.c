#include "lib/common.h"

int main(int argc, char *argv[]) {
  options("cCbdfhimMnrRsuVzk:o:t:");
  if (flag('o'))
    if (!freopen(lastarg('o'), "w", stdout)) return errno;
  if (flag('m')) {
    while (
  }
  return errno;
}
