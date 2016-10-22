#include "common.h"

void *memdup(const void *s, size_t n) {
  void *dest = malloc(n);
  return dest ? memcpy(dest, s, n) : NULL;
}

mode_t getumask() {
  mode_t mask = umask(0);
  umask(mask);
  return mask;
}
