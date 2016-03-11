#include <string.h>
#include <stdint.h>
#include <stdio.h>
char *flags[63];

#define isnum(x)   (x >= 48 && x <= 57)
#define isupper(x) (x >= 64 && x <= 90)
#define islower(x) (x >= 97 && x <= 122)
#define opt(x) (isnum(x)   ? x - 48 :   \
                isupper(x) ? x - 55 :   \
                islower(x) ? x - 61 : 63)

int optind, opterr;

int parseopts(int argc, char *argv[], const char *opts) {
  char *valid = ":ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
  if (strlen(opts) != strspn(opts, valid)) return -1;
  valid++;

  int64_t needflag = 0;
  int i, j;
  for (i = 0; opts[i]; i++)
    if (opts[i+1] == ':')
      needflag |= 1 << opt(opts[i+1]);

  char c;
  for (i = 0; i < argc && argv[i][0] == '-' && argv[i][1]; i++) {
    if (argv[i][1] == '-' && !argv[i][2]) {
      optind = i + 1;
      return 0;
    }
    for (j = 1; (c = argv[i][j]); j++) {
      if (strchr(valid, c)) {
        if (needflag & 1 << opt(c)) {
          if (argv[i][j+1])
            flags[opt(c)] = &argv[i][j+1];
          else if (argv[++i])
            flags[opt(c)] = &argv[i][0];
          else {
            if (opterr)
              fprintf(stderr, "Missing argument for option: -%c\n", c);
            return ':';
          }
        }
        else flags[opt(c)] = (char *)1;
      }
      else {
        if (opterr)
          fprintf(stderr, "Unknown option: -%c\n", c);
        return '?';
      }
    }
  }
  optind = i;
  return 0;
}
