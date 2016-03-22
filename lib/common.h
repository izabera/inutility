#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stddef.h>
#include <stdint.h>
#include <libgen.h>
#include <signal.h>
#include <time.h>

#include "flags.h"

#define arrsize(a) sizeof(a)/sizeof(*a)
#define min(a,b) ((a) < (b) ? (a) : (b))
#define max(a,b) ((a) > (b) ? (a) : (b))

struct str {
  unsigned char *str;
  size_t len;
};

char *memdup(const char *mem, size_t len);
