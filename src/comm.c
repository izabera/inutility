#include "lib/common.h"

int main(int argc, char *argv[]) {
  options("123", .argleast = 2, .arglessthan = 3);
  FILE *file[2] = {
    [0] = strcmp("-", argv[1]) ? fopen(argv[1], "r") : stdin,
    [1] = strcmp("-", argv[2]) ? fopen(argv[2], "r") : stdin,
  };
  if (!file[0] || !file[1]) return errno;

  const char *formats[3][8] = { // premade printf formats for all cases
    [0][0] = "%s", [1][0] = "\t%s", [2][0] = "\t\t%s", // -+
    [0][1] = ""  , [1][1] = "%s"  , [2][1] = "\t%s"  , //  |
    [0][2] = "%s", [1][2] = ""    , [2][2] = "\t%s"  , //  +- gcc & clang assign the same pointer
    [0][3] = ""  , [1][3] = ""    , [2][3] = "%s"    , //     to elements with the same value
    [0][4] = "%s", [1][4] = "\t%s", [2][4] = ""      , //     at any optimization level
    [0][5] = ""  , [1][5] = "%s"  , [2][5] = ""      , //  +- so this is small-ish
    [0][6] = "%s", [1][6] = ""    , [2][6] = ""      , //  |
    [0][7] = ""  , [1][7] = ""    , [2][7] = ""      , // -+
  };
  int mask = flag('1') | !!flag('2') << 1 | !!flag('3') << 2;

  char *line[2] = { 0 };
  size_t len[2] = { 0 };
  ssize_t read[2] = {
    [0] = getline(&line[0], &len[0], file[0]),
    [1] = getline(&line[1], &len[1], file[1]),
  };

  while (read[0] != -1 && read[1] != -1) {
    int cmp = strcmp(line[0], line[1]);
    printf(formats[cmp == 0 ? 2 : cmp < 0 ? 0 : 1][mask], line[cmp > 0]);
    if (cmp == 0) {
      read[0] = getline(&line[0], &len[0], file[0]);
      read[1] = getline(&line[1], &len[1], file[1]);
    }
    else if (cmp < 0) read[0] = getline(&line[0], &len[0], file[0]);
    else              read[1] = getline(&line[1], &len[1], file[1]);
  }

  int which = read[0] == -1;
  while (read[which] != -1) {
    printf(formats[which][mask], line[which]);
    read[which] = getline(&line[which], &len[which], file[which]);
  }
  return errno;
}
