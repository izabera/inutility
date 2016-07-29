#include "lib/common.h"

int main(int argc, char *argv[]) {
  options("abcdfilosxvA:j*N*t:w*");
  // traditional form is dumb and not supported
  struct format { char type, size; } format = { 'o', '2' };
  if (flag('a')) format.type = 'a';
  if (flag('b')) format = (struct format) { 'o', '1' };
  if (flag('c')) format.type = 'c';
  if (flag('d')) format = (struct format) { 'u', '2' };
  if (flag('f')) format = (struct format) { 'f', 'F' };
  if (flag('i')) format = (struct format) { 'd', 'I' };
  if (flag('l')) format = (struct format) { 'd', 'L' };
  if (flag('o')) format = (struct format) { 'o', '2' };
  if (flag('s')) format = (struct format) { 'd', '2' };
  if (flag('x')) format = (struct format) { 'x', '2' };
  if (flag('t')) {
    if (lastarg('t')[0] && strchr("acdfoux", lastarg('t')[0]))
      format = (struct format) { lastarg('t')[0], lastarg('t')[1] };
    else return -1;
  }
  char address = 'o';
  if (flag('A')) {
    if (lastarg('o')[0] && strchr("doxn", lastarg('o')[0]))
      address = lastarg('o')[0];
    else return -1;
  }

  FILE *fileptr = stdin;
  if (argc == 1) {
    argv[0] = "-";
    goto inner;
  }
  while (*++argv) {
         if (argv[0][0] == '-' && argv[0][1] == 0) fileptr = stdin;
    else if (!(fileptr = fopen(argv[0], "r"))) continue;
inner:
    if (fileptr != stdin) fclose(fileptr);
  }
  return errno;
}
