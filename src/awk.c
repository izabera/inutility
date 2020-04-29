#include "lib/common.h"

/*
#define flag(x)    (flags[opt(x)].count)
#define lastarg(x) (flags[opt(x)].args[flag(x)-1])
#define lastnum(x) (flags[opt(x)].nums[flag(x)-1])
*/

static htable *h;
#define hset(...) hset(h, __VA_ARGS__)
#define hget(...) hget(h, __VA_ARGS__)
#define hdel(...) hdel(h, __VA_ARGS__)

int main(int argc, char *argv[]) {
  options("f:F:v:");

  struct str script;
  if (flag('f')) {
    FILE *memstream = open_memstream(&script.str, &script.len);
    for (size_t i = 0; i < flag('f'); i++) {
      FILE *f = fopen(flags[opt('f')].args[i], "r");
      for (int c; (c = fgetc_unlocked(f)) != -1; )
        fputc_unlocked(c, memstream);
      fclose(f);
    }
    fclose(memstream);
  }
  else
    script.str = argv[1];

  h = hcreate(64);

  hset("FS", flag('F') ? lastarg('F') : " ");
  for (size_t i = 0; i < flag('v'); i++) {
    char *ptr = strchr(flags[opt('v')].args[i], '=');
    if (ptr) {
      *ptr = 0;
      hset(flags[opt('v')].args[i], ptr+1);
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
  return 0;
}
