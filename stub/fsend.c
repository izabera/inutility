#include <stdio.h>

size_t fsend_unlocked(FILE *in, FILE *out, size_t size) {
  size_t ret = 0, read, write;
  char buf[size][BUFSIZ];
  do {
    read = fread_unlocked(buf, size, BUFSIZ, in);
    write = fwrite_unlocked(buf, size, read, out);
    ret += write;
  } while (read && write == read);
  return ret;
}

size_t fsend(FILE *in, FILE *out, size_t size) {
  flockfile(in);
  flockfile(out);
  size_t ret = fsend_unlocked(in, out, size);
  funlockfile(in);
  funlockfile(out);
  return ret;
}
