#include "lib/common.h"

int main(int argc, char *argv[]) {
  options("ai");
  char *mode = flag('a') ? "a" : "w";

  if (flag('i')) signal(SIGINT, SIG_IGN);

  int i = 1;
  FILE *files[argc];
  files[0] = stdout;
  setvbuf(stdout, NULL, _IONBF, 0);
  while (*++argv)
    if ((files[i] = fopen(argv[0], mode))) setvbuf(files[i++], NULL, _IONBF, 0);
    else argc--;

  sequential(0);
  ssize_t size;
  char buf[IBUFSIZ];

  while ((size = read(0, buf, sizeof(buf))) > 0)
    for (i = 0; i < argc; i++)
      fwrite_unlocked(buf, 1, size, files[i]);

  return errno;
}
