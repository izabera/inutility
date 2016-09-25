#include "common.h"

void copyfd(int in, int out) {
  int saverrno = errno;
  ssize_t size;
  while ((size = sendfile(out, in, NULL, 1 << 24)) > 0) { } // try sendfile first and fall back
  errno = saverrno;
  if (size) {// -1
    char buf[IBUFSIZ];
    while ((size = read(in, buf, sizeof(buf))) > 0)
      for (ssize_t w = 0, off = 0; w != -1 && (size -= w); w = write(out, buf+off, size))
        off += w; // maybe use stdio?
  }
}
