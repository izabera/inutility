#include "lib/common.h"

static int delim;
static void miniprintf(const char *fmt, const char *str, size_t len) {
  for ( ; *fmt == '\t'; fmt++) putchar_unlocked('\t');
  if (*fmt) fwrite_unlocked(str, 1, len, stdout);
  if (str[len-1] != delim) putchar_unlocked(delim);
}

int main(int argc, char *argv[]) {
  options("123z", .argleast = 2, .arglessthan = 3);
  FILE *file[2] = {
    [0] = strcmp("-", argv[1]) ? fopen(argv[1], "r") : stdin,
    [1] = strcmp("-", argv[2]) ? fopen(argv[2], "r") : stdin,
  };
  if (!file[0] || !file[1]) return errno;

  const char *formats[3][8] = { // premade printf formats for all cases
    [0][0] = "%", [1][0] = "\t%", [2][0] = "\t\t%", // -+
    [0][1] = "" , [1][1] = "%"  , [2][1] = "\t%"  , //  |
    [0][2] = "%", [1][2] = ""   , [2][2] = "\t%"  , //  +- gcc & clang assign the same pointer
    [0][3] = "" , [1][3] = ""   , [2][3] = "%"    , //     to elements with the same value
    [0][4] = "%", [1][4] = "\t%", [2][4] = ""     , //     at any optimization level
    [0][5] = "" , [1][5] = "%"  , [2][5] = ""     , //  +- so this is small-ish
    [0][6] = "%", [1][6] = ""   , [2][6] = ""     , //  |
    [0][7] = "" , [1][7] = ""   , [2][7] = ""     , // -+
  };
  int mask = flag('1') | !!flag('2') << 1 | !!flag('3') << 2;
  delim = flag('z') ? 0 : '\n';

  char *line[2] = { 0 };
  size_t len[2] = { 0 };
  ssize_t read[2] = {
    [0] = getdelim(&line[0], &len[0], delim, file[0]),
    [1] = getdelim(&line[1], &len[1], delim, file[1]),
  };

  while (read[0] != -1 && read[1] != -1) {
    int cmp = read[0] == read[1] ? memcmp(line[0], line[1], read[0])     :
              read[0]  < read[1] ? memcmp(line[0], line[1], read[0]) | 1 : // not 0
                                   memcmp(line[0], line[1], read[1]) | 1 ;
    miniprintf(formats[cmp == 0 ? 2 : cmp < 0 ? 0 : 1][mask], line[cmp > 0], read[cmp > 0]);
    if (cmp == 0) {
      read[0] = getdelim(&line[0], &len[0], delim, file[0]);
      read[1] = getdelim(&line[1], &len[1], delim, file[1]);
    }
    else if (cmp < 0) read[0] = getdelim(&line[0], &len[0], delim, file[0]);
    else              read[1] = getdelim(&line[1], &len[1], delim, file[1]);
  }

  int which = read[0] == -1;
  while (read[which] != -1) {
    miniprintf(formats[which][mask], line[which], read[which]);
    read[which] = getdelim(&line[which], &len[which], delim, file[which]);
  }
  return errno;
}
