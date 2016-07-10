#include "lib/common.h"

int main(int argc, char *argv[]) {
  options("cr:s*", .argleast = 1);
  int file;
  size_t siz;
  struct stat st;
  if (flag('s')) siz = lastnum('s');
  else if (flag('r')) {
    if (stat(lastarg('r'), &st) == -1) return errno;
    siz = st.st_size;
  }
  else return -1;

  while (*++argv) {
         if (argv[0][0] == '-' && argv[0][1] == 0) file = 0;
    else if ((file = open(argv[0], O_WRONLY|((!flag('c'))*O_CREAT), 0666)) == -1) continue;
    ftruncate(file, siz);
    if (file) close(file);
  }
  return errno;
}
