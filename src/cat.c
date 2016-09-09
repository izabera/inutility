#include "lib/common.h"

#define hat(x) (x < 32)
#define lowmeta(x) (x > 127 && x < 160)
#define himeta(x) (x > 159)
int main(int argc, char *argv[]) {
  char buf[IBUFSIZ];
  int file = 0, c;
  ssize_t size;
  /* todo: -b -s */
  options("AeEntTuv");

  if (flag('A')) flag('v') = flag('E') = flag('T') = 1;
  if (flag('e')) flag('v') = flag('E') = 1;
  if (flag('t')) flag('v') = flag('T') = 1;
  if (flag('u')) setvbuf(stdout, NULL, _IONBF, 0);

  char line = 1, anyflag = !!(flag('E') | flag('n') | flag('T') | flag('v'));

  size_t linecount = 1;
  FILE *fileptr = stdin;
  if (argc == 1)
    goto inner;

  while (*++argv) {
         if (argv[0][0] == '-' && argv[0][1] == 0) { file = 0; fileptr = stdin; }
    else if ((file = open(argv[0], O_RDONLY)) == -1) continue;
    else fileptr = fdopen(file, "r");
inner:
    if (anyflag) {
      while ((c = fgetc_unlocked(fileptr)) != EOF) {
             if (line)      { if (flag('n')) printf("%6lu\t", linecount); line = 0; }
             if (c == '\t')   printf(flag('T') ? "^I"    : "\t");
        else if (c == '\n') { printf(flag('E') ? "$\n"   : "\n"); linecount++; line = 1; }
        else if (hat(c))      printf(flag('v') ? "^%c"   : "%c", c + !!flag('v') * 64);
        else if (c == 127)    printf(flag('v') ? "^?"    : "\177");
        else if (c == 255)    printf(flag('v') ? "M-^?"  : "\377");
        else if (lowmeta(c))  printf(flag('v') ? "M-^%c" : "%c", c - 64);
        else if (himeta(c))   printf(flag('v') ? "M-%c"  : "%c", c - 128);
        else                  putchar_unlocked(c);
      }
      fflush_unlocked(stdout);
    }
    else {
      while ((size = read(file, buf, sizeof(buf))) > 0)
        for (ssize_t w = 0; w != -1 && (size -= w); w = write(1, buf, size)) ;
    }
    if (fileptr != stdin) fclose(fileptr);
  }
  return errno;
}
