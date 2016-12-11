#include <stdio.h>

size_t fsend_unlocked(FILE *in, FILE *out) {
  size_t ret = 0, read, write;
  char buf[BUFSIZ];
  do {
    read = fread_unlocked(buf, 1, BUFSIZ, in);
    write = fwrite_unlocked(buf, 1, read, out);
    ret += write;
  } while (read && write == read);
  return ret;
}

size_t fsend(FILE *in, FILE *out) {
  flockfile(in);
  flockfile(out);
  size_t ret = fsend_unlocked(in, out);
  funlockfile(in);
  funlockfile(out);
  return ret;
}
