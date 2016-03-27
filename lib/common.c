#include "common.h"

char *memdup(const char *s, size_t n) {
  char *dest = malloc(n);
  if (!dest) return NULL;
  memcpy(dest, s, n);
  return dest;
}

mode_t getumask() {
  mode_t mask = umask(0);
  umask(mask);
  return mask;
}
