#include "lib/common.h"

#include "lib/factor.data"

static int composite_miller_rabin(uint8_t a, uint64_t d, uint64_t n, uint8_t s) {
  if (powermod(a, d, n) == 1) return 0;
  for (uint8_t i = 0; i < s; i++)
    if (powermod(a, (1<<i)*d, n) == 1) return 0;
  return 1;
}

static int deterministic_miller_rabin(uint64_t num) {
  uint64_t d = num - 1;
  uint8_t s = 0;
  while (d % 2 == 0) {
    s++;
    d /= 2;
  }

  uint8_t arr[] = { 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37 };
  for (uint8_t i = 0; i < arrsize(arr); i++)
    if (composite_miller_rabin(arr[i], d, num, s)) return 0;
  return 1;
}

static void factor(const char* str) {
  uint64_t num = strtoull(str, NULL, 10), factor = 1;
  uint8_t increment = primes[arrsize(primes)-1] % 6 == 1 ? 2 : 4;
  printf("%" PRIu64 ":", num);
#if 0
  size_t i = 0;
#define getprime() (i < arrsize(primes) ? primes[i++] : factor + (increment ^= 6))
  while (factor * factor <= num) {
    if (num % (factor = getprime()) == 0) {
      do {
        printf(" %" PRIu64, factor);
        num /= factor;
      } while (num % factor == 0);
    }
  }
#else
  size_t i = 0;
#define getprime() (i < arrsize(primes) ? primes[i++] : factor + (increment ^= 6))
  while (factor * factor <= num && factor <= 100000) {
    while (num % (factor = getprime()) == 0) {
      printf(" %" PRIu64, factor);
      num /= factor;
    }
  }

  // trial division gets boring, is this a prime by any chance?
  if (deterministic_miller_rabin(num)) goto lastprime;
  puts("here");
  while (factor * factor <= num) {
    if (num % (factor = getprime()) == 0) {
      do {
        printf(" %" PRIu64, factor);
        num /= factor;
      } while (num % factor == 0);
    }
  }

#if 0
  // we're factoring a 64 bit number and its smallest factor is > 100k
  // if it's a perfect square, we're done
  uint64_t tmp = isqrt(num);
  if (tmp * tmp == num) {
    printf(" %" PRIu64, num = tmp);
    goto lastprime;
  }
  // of course if it's a perfect cube, we're done too
  tmp = icbrt(num);
  if (tmp * tmp * tmp == num) {
    printf(" %" PRIu64 " %" PRIu64, tmp, num = tmp);
    goto lastprime;
  }

  // no luck, let's use squfof
#endif
#endif
lastprime: if (num > 1) printf(" %" PRIu64, num);
  putchar_unlocked('\n');
}

int main(int argc, char *argv[]) {
  options("");
  if (argc == 1) {
    ssize_t read;
    size_t len = 0;
    char *line = NULL;
    while ((read = getline(&line, &len, stdin)) > 0) factor(line);
  }
  else while (*++argv) factor(*argv);
  return errno;
}
