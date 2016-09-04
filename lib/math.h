#include <stdint.h>

static inline uint8_t ilog2(uint64_t n) {
  // todo: fallback
  return 64 - __builtin_clzll(n) - 1;
}

static inline uint64_t isqrt(uint64_t n) {
  if (n == 0) return 0;
  uint8_t halflog = ilog2(n) / 2;
  uint64_t hi = UINT64_C(1) << (halflog+1), lo = UINT64_C(1) << halflog, mid;
  if (n == UINT64_C(-1)) return hi;
  while (hi > lo+1) {
    mid = (hi + lo) / 2;
    if (mid * mid == n) return mid;
    if (mid * mid < n) lo = mid;
    else hi = mid;
  }
  return lo;
}

static inline uint64_t powermod(uint64_t base, uint64_t exp, uint64_t mod) {
  uint64_t res = 1;
  for ( ; exp; exp /= 2) {
    if (exp % 2 == 1) res = (res * base) % mod;
    base = (base * base) % mod;
  }
  return res;
}
