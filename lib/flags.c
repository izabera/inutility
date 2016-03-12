#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include "flags.h"

#define optpush(opt, arg) do {                                                \
  opt.count++;                                                                \
  if (!(opt.args = realloc(opt.args, sizeof(char*) * opt.count))) return '@'; \
  opt.args[opt.count-1] = arg;                                                \
} while (0)

int parseoptind, parseopterr = 1;
struct flags flags[63];

int parseopts(int argc, char *argv[], const char *opts) {
  char *valid = ":-ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
  if (strlen(opts) != strspn(opts, valid)) return -1;
  valid++;
  if (*opts == ':') {
    opts++;
    parseopterr = 0;
  }
  if (*opts == ':') return -1;

  int64_t needflag = 0;
  int i, j;
  for (i = 0; opts[i]; i++)
    if (opts[i+1] == ':') needflag |= 1 << opt(opts[i]);

  char c;
  for (i = 0; i < argc && argv[i][0] == '-' && argv[i][1]; i++) {
    if (argv[i][1] == '-' && !argv[i][2]) {
      i++;
      break;
    }
    for (j = 1; (c = argv[i][j]); j++) {
      if (strchr(opts, c)) {
        if (needflag & 1 << opt(c)) {
               if (argv[i][j+1]) optpush(flags[opt(c)], &argv[i][j+1]);
          else if (++i < argc)   optpush(flags[opt(c)], &argv[i][0]);
          else {
            if (parseopterr) fprintf(stderr, "Missing argument for option: -%c\n", c);
            return ':';
          }
          break;
        }
        else flags[opt(c)].count++;
      }
      else {
        if (parseopterr) fprintf(stderr, "Unknown option: -%c\n", c);
        return '?';
      }
    }
  }
  parseoptind = i;
  return 0;
}
