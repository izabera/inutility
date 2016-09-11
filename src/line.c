#include "lib/common.h"

// from solaris -- may be useful?  for rc?
int main(int argc, char *argv[]) {
  options("", .arglessthan = 1);
  int seekable = fseek(stdin, 0, SEEK_CUR) != -1, c;
  errno = 0;
  if (!seekable) setvbuf(stdin, NULL, _IONBF, 0);
  while ((c = getchar_unlocked()) != EOF && c != '\n') putchar_unlocked(c);
  fflush_unlocked(stdin);
  if (seekable) fseek(stdin, 0, SEEK_CUR);
  putchar_unlocked('\n');
  return (c != '\n') | errno;
}
