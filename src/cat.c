#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

#include "lib/flags.h"

int main(int argc, char *argv[]) {
  char buf[BUFSIZ];
  int file;
  ssize_t size;
  if (parseopts(argc, argv, ":u")) return -1;

  if (argc == 1) {
    file = 0;
    goto inner;
  }
  while (*++argv) {
    if (argv[0][0] == '-' && argv[0][1] == 0) file = 0;
    else if (!(file = open(argv[0], O_RDONLY))) continue;
inner:
    while ((size = read(file, buf, BUFSIZ)) > 0) {
      write(1, buf, size);
    }
    close(file);
  }
  return 0;
}
