#include "lib/common.h"

int main(int argc, char *argv[]) {
  options("ai");
  int openflags = O_WRONLY | O_CREAT;
  if (flag('a')) openflags |= O_APPEND;
  else openflags |= O_TRUNC;

  if (flag('i')) signal(SIGINT, SIG_IGN);

  int files[argc], i = 0;
  files[0] = 1;
  while (*++argv)
    files[++i] = open(argv[0], openflags, 0666);

  ssize_t size;
  char buf[BUFSIZ];

  while ((size = read(0, buf, BUFSIZ)) > 0)
    for (i = 0; i < argc; i++)
      if (files[i] > 0)
        for (ssize_t w = 0, s = size; w != -1 && (s -= w); w = write(files[i], buf, s)) ;

  return errno;
}
