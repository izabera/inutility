#include "lib/common.h"

#define hat(x) (x < 32)
#define lowmeta(x) (x > 127 && x < 160)
#define himeta(x) (x > 159)
int main(int argc, char *argv[]) {
  int file = 0, c;
  /* todo: -b */
  options("AeEntsTuv");

  if (flag('A')) flag('v') = flag('E') = flag('T') = 1;
  if (flag('e')) flag('v') = flag('E') = 1;
  if (flag('t')) flag('v') = flag('T') = 1;
  if (flag('u')) setvbuf(stdout, NULL, _IONBF, 0);

  if (flag('n')) flag('s') = 0; // gnu cat does this
  char line = 1, anyflag = !!(flag('E') | flag('n') | flag('T') | flag('v') | flag('s'));

  size_t linecount = 1;
  FILE *fileptr = stdin;
  if (argc == 1)
    goto inner;

  while (*++argv) {
         if (argv[0][0] == '-' && argv[0][1] == 0) { file = 0; fileptr = stdin; }
    else if ((file = open(argv[0], O_RDONLY)) == -1) continue;
    else fileptr = fdopen(file, "r");
inner:
    sequential(file);
    if (anyflag) {
#define pc putchar_unlocked // these silly macros are much faster than printf
#define ps(x) fputs_unlocked(x, stdout)
      while ((c = fgetc_unlocked(fileptr)) != EOF) { loop:
             if (line)      { if (flag('n')) printf("%6lu\t", linecount); line = 0; }
             if (c == '\t')   if (flag('T')) ps("^I"); else pc('\t');
        else if (c == '\n') { if (flag('E')) pc('$'); pc('\n'); linecount++; line = 1;
          if (flag('s')) {
            for (int i = 0; (c = fgetc_unlocked(fileptr)) == '\n'; ) if (!i++) pc('\n');
            if (c == EOF) break;
            goto loop;
          }
        }
        else if (hat(c))    { if (flag('v')) pc('^'); pc(c + !!flag('v') * 64); }
        else if (c == 127)    if (flag('v')) ps("^?"); else pc('\177');
        else if (c == 255)    if (flag('v')) ps("M-^?"); else pc('\377');
        else if (lowmeta(c))  if (flag('v')) ps(((char[]) { 'M', '-', '^', c - 64, 0 })); else pc(c);
        else if (himeta(c))   if (flag('v')) ps(((char[]) { 'M', '-', c - 128, 0 })); else pc(c);
        else                  pc(c);
      }
      fflush_unlocked(stdout);
    }
    else copyfd(file, 1);
    if (fileptr != stdin) fclose(fileptr);
  }
  return errno;
}
