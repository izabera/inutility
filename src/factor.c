#include "lib/common.h"

#include "lib/factor.data"
static void factor(const char* str) {
  uint64_t num = strtoull(str, NULL, 10), factor = 0;
  uint8_t increment = primes[arrsize(primes)-1] % 6 == 1 ? 2 : 4;
  printf("%" PRIu64 ":", num);
  if (num < 2) goto end;
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
  if (num > 1) printf(" %" PRIu64, num);
end: putchar_unlocked('\n');
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
