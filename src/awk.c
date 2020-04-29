#include "lib/common.h"

/*
#define flag(x)    (flags[opt(x)].count)
#define lastarg(x) (flags[opt(x)].args[flag(x)-1])
#define lastnum(x) (flags[opt(x)].nums[flag(x)-1])
*/


static void slurp(char *file, FILE *memstream) {
  FILE *f = fopen(file, "r");
  for (int c; (c = fgetc_unlocked(f)) != -1; )
    fputc_unlocked(c, memstream);
  fclose(f);
}

int main(int argc, char *argv[]) {
  options("f:F:v:");

  struct str script;
  if (flag('f')) {
    FILE *memstream = open_memstream(&script.str, &script.len);
    for (size_t i = 0; i < flag('f'); i++) slurp(flags[opt('f')].args[i], memstream);
    fclose(memstream);
  }
  else
    script.str = argv[1];

  void *root = 0;
#define vars(x) tsearch(strdup(x), &root, strcmp)
#define delete(x) tdelete((x), &root, strcmp)
  vars("FS") = strdup(flag('F') ? lastarg('F') : " ");
  for (size_t i = 0; i < flag('v'); i++) {
    char *ptr = strchr('=', flags[opt('v')].args[i]);
    if (ptr) {
      *ptr = 0;
      vars(flags[opt('v')].args[i]) = strdup(ptr+1);
    }
  }

  /*

  size_t linecount = 1;
  FILE *fileptr = stdin;
  if (argc == 1)
    goto inner;

  while (*++argv) {
    if (argv[0][0] == '-' && argv[0][1] == 0) { file = 0; fileptr = stdin; }
    else if ((file = open(argv[0], O_RDONLY)) == -1) continue;
    else fileptr = fdopen(file, "r");
inner:

    if (argc == 1)
          goto inner;

      while (*++argv) {

  else if (argc > 1)


  

  */
}
