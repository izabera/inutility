#include <stdint.h>

uint8_t ilog2(uint64_t n) { return 64 - __builtin_clzll(n) - 1; }

uint64_t isqrt(uint64_t n) {
  // bisection
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

uint64_t icbrt(uint64_t n) {
  // as above
  if (n == 0) return 0;
  uint8_t thirdlog = ilog2(n) / 3;
  uint64_t hi = UINT64_C(1) << (thirdlog+1), lo = UINT64_C(1) << thirdlog, mid;
  if (n == UINT64_C(-1)) return hi;
  while (hi > lo+1) {
    mid = (hi + lo) / 2;
    if (mid * mid * mid == n) return mid;
    if (mid * mid * mid < n) lo = mid;
    else hi = mid;
  }
  return lo;
}

uint64_t powermod(uint64_t base, uint64_t exp, uint64_t mod) {
  // with exp by squaring
  uint64_t res = 1;
  for ( ; exp; exp /= 2) {
    if (exp % 2 == 1) res = (res * base) % mod;
    base = (base * base) % mod;
  }
  return res;
}

uint64_t gcd(uint64_t a, uint64_t b) {
  // binary gcd
  if (!a) return b;
  if (!b) return a;

  uint8_t shift = __builtin_ctzll(a|b);
  a >>= shift;
  b >>= shift;
  a >>= __builtin_ctzll(a);
  do {
    b >>= __builtin_ctzll(b);
    if (a > b) { uint64_t tmp = a; a = b; b = tmp; }
    b -= a;
  } while (b);

  return b << shift;
}
