#include "lib/common.h"

int main(int argc, char *argv[]) {
  options("lsn*", .argleast = 1, .arglessthan = 5);
  FILE *file1, *file2 = stdin;
  size_t skip1 = 0, skip2 = 0, limit = flag('n') ? lastnum('n') : -1;
  switch (argc) {
    default:
    case 5: skip2 = parsebyte(argv[4]);
    case 4: skip1 = parsebyte(argv[3]);
    case 3: if (!strcmp("-", argv[2])) file2 = stdin;
            else if (!(file2 = fopen(argv[2], "r"))) return -1;
    case 2: if (!strcmp("-", argv[1])) file1 = stdin;
            else if (!(file1 = fopen(argv[1], "r"))) return -1;
            if (argc == 2) argv[2] = "-";
  }

  if (file1 == file2) return 0; // cmp - or cmp - -

  char buf[BUFSIZ];
  size_t tmp;
  if (skip1 && fseek(file1, skip1, SEEK_CUR))
    while (skip1) {
      if ((tmp = fread_unlocked(buf, 1, min(skip1, sizeof(buf)), file1)) <= 0) break;
      skip1 -= tmp;
    }
  if (skip2 && fseek(file2, skip2, SEEK_CUR))
    while (skip2) {
      if ((tmp = fread_unlocked(buf, 1, min(skip2, sizeof(buf)), file2)) <= 0) break;
      skip2 -= tmp;
    }
  errno = 0;

  int c1, c2, ret = 0;
  for (size_t pos = 0, lineno = 0; pos < limit; pos++) {
    c1 = getc_unlocked(file1);
    c2 = getc_unlocked(file2);

    if (c1 == c2) {
      if (c1 == EOF) break;
      if (c1 == '\n') lineno++;
      continue;
    }
    ret = 1;
    if (c1 == EOF || c2 == EOF) {
      if (!flag('s')) printf("cmp: EOF on %s\n", c1 == EOF ? argv[1] : argv[2]);
      break;
    }
    else if (flag('l')) printf("%zu %o %o\n", pos+1, c1, c2);
    else {
      if (!flag('s')) printf("%s %s differ: char %zu, line %zu\n", argv[1], argv[2], pos+1, lineno+1);
      break;
    }
  }
  return errno | ret;
}
