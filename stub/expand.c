#include "lib/common.h"

int main(int argc, char *argv[]) {
  options("it:");
  int c, initial = 1;
  size_t col = 0, tab = 8, *tabs = NULL, tabs_size = 1;
  if (flag('t')) {
    if (!(tabs = malloc(sizeof(size_t)))) return errno;
    do {
      tabs[tabs_size-1] = strtoull(lastarg('t'), NULL, 10);
      if (tabs_size != 1 && tabs[tabs_size-1] <= tabs[tabs_size-2]) return -1;
      if (!(tabs = realloc(tabs, ++tabs_size * sizeof(size_t)))) return errno;
      strsep(&lastarg('t'), " \t,");
    } while (lastarg('t'));
    if (tabs_size-- == 1) {
      flag('t') = 0;
      if (!(tab = tabs[0])) return -1;
    }
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
    while ((c = getc_unlocked(fileptr)) != EOF) {
      switch (c) {
                   // todo \b
        Case '\n': col = 0;
                   initial = 1;
                   putchar_unlocked('\n');
        Default  : putchar_unlocked(c);
                   initial = col++ == 0;
                   printf("initial: %d col %zu\n", initial, col);
        Case '\t':
                   printf("t initial: %d col %zu\n", initial, col);
                   if (flag('i') && !initial) {
                     putchar_unlocked('\t');
                     col++;
                     break;
                   }
                   if (flag('t')) {
                     size_t *tmp = tabs, used = 0;
                     size_t i = 0;
                     for (; i < tabs_size; i++)
                       if (col < tabs[i]) break;
                     /*putchar('{');*/
                     /*printf("col: %zu   tmp = %zu\n", col, *tmp);*/
                     while (*tmp < col && used) {
                       tmp++;
                     /*printf("col: %zu   tmp = %zu\n", col, *tmp);*/
                     }
                     printf("col: %zu   tmp = %zu\n", col, *tmp);
                     /*putchar('}');*/
                     do {
                       putchar_unlocked(' ');
                     printf("col: %zu   tmp = %zu\n", col, *tmp);
                     sleep(1);
                     } while (++col < *tmp);
                   }
                   else { do { putchar_unlocked(' '); } while (++col % tab); }
      }
    }
    if (fileptr != stdin) fclose(fileptr);
  }
  return errno;
}
