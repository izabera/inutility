#include <string.h>
#include <stdint.h>
#include <stdio.h>

#include "flags.h"

int parseopts(int argc, char *argv[], const char *opts) {
  char *valid = ":ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
  if (strlen(opts) != strspn(opts, valid)) return -1;
  valid++;

  int64_t needflag = 0;
  int i, j;
  for (i = 0; opts[i]; i++)
    if (opts[i+1] == ':')
      needflag |= 1 << opt(opts[i]);

  char c;
  for (i = 0; i < argc && argv[i][0] == '-' && argv[i][1]; i++) {
    if (argv[i][1] == '-' && !argv[i][2]) {
      i++;
      break;
    }
    for (j = 1; (c = argv[i][j]); j++) {
      if (strchr(valid, c)) {
        if (needflag & 1 << opt(c)) {
          if (argv[i][j+1])
            flags[opt(c)] = &argv[i][j+1];
          else if (++i < argc)
            flags[opt(c)] = &argv[i][0];
          else {
            if (opterr)
              fprintf(stderr, "Missing argument for option: -%c\n", c);
            return ':';
          }
          break;
        }
        else flags[opt(c)] += 1;
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
