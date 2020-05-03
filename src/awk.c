#include "lib/common.h"

static htable *h;
#define hset(...) hset(h, __VA_ARGS__)
#define hget(...) hget(h, __VA_ARGS__)
#define hdel(...) hdel(h, __VA_ARGS__)

int main(int argc, char *argv[]) {
  options("f:F:v:", .argleast = 1);

  struct str script;
  if (flag('f')) {
    FILE *memstream = open_memstream(&script.str, &script.len);
    for (size_t i = 0; i < flag('f'); i++) {
      FILE *f;
           if (!strcmp(flags[opt('f')].args[i], "-")) f = stdin;
      else if (!(f = fopen(flags[opt('f')].args[i], "r"))) continue;
      for (int c; (c = fgetc(f)) != -1; )
        fputc(c, memstream);
      if (f != stdin) fclose(f);
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

  return 0;
}
