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
#include <inttypes.h>
#include <libgen.h>
#include <signal.h>
#include <time.h>
#include <limits.h>
#include <sys/sysinfo.h>
#include <utmpx.h>
#include <time.h>
#include <stdio.h>
#include <sys/sysinfo.h>
#include <utmpx.h>
#include <time.h>

#include "flags.h"
#include "parsenumb.h"
#include "parsemode.h"
#include "memspn.h"
#include "unescape.h"
#include "uptime.h"

#define arrsize(a) (sizeof(a)/sizeof(*a))
#define min(a,b) ((a) < (b) ? (a) : (b))
#define max(a,b) ((a) > (b) ? (a) : (b))

struct str {
  unsigned char *str;
  size_t len;
};

char *memdup(const char *mem, size_t len);
mode_t getumask();


// "gross" -- people in ##c
#define casebin       '0': case '1'
#define caseoct  casebin : case '2': case '3': case '4': case '5': case '6': case '7'
#define casedec  caseoct : case '8': case '9'
#define caselhex casedec : case 'a': case 'b': case 'c': case 'd': case 'e': case 'f'
#define caseuhex casedec : case 'A': case 'B': case 'C': case 'D': case 'E': case 'F'
#define casehex  caselhex: case 'A': case 'B': case 'C': case 'D': case 'E': case 'F'
