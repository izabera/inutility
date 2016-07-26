#include "lib/common.h"

int main(int argc, char *argv[]) {
  options("mcCbdhfinruRMVo:zst:k:");
  if (flag('o'))
    if (!freopen(lastarg('o'), "w", stdout)) return errno;
  if (flag('m')) {
    while (
  }
  return errno;
}
