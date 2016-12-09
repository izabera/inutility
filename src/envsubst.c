#include "lib/common.h"

// parsing is hard

// parse a var after $
// returns a string that doesn't need to be freed
char *getvar(FILE *input, FILE *fail) {
  int braces = 0, c;
  if ((c = getc_unlocked(input)) == EOF) return NULL;
  if (c == '{') {
    putc_unlocked(c, fail);
    braces = 1;
  }
  else ungetc(c, input);

  if ((c = getc_unlocked(input)) == EOF) return NULL;
  if (!isalpha(c) && c != '_') {
    putc_unlocked(c, fail);
    return NULL;
  }

  ungetc(c, input);
  char *var;
  scanf("%m[A-Za-z0-9_]", &var);
  if (!var) return NULL;
  fputs_unlocked(var, fail);
  char *value = getenv(var);
  if (!value) value = "";
  free(var);

  if (braces) {
    if ((c = getchar_unlocked()) == EOF) return NULL;
    if (c != '}') {
      ungetc(c, input);
      putc_unlocked(c, fail);
      return NULL;
    }
  }
  return value;
}

int main(int argc, char *argv[]) {
  options("");
  /*options("v", .arglessthan = 2);*/
  /*if (flag('v') && argc == 1) return -1;*/
  /*if (flag('v')) {*/
  /*}*/

  for (int c; (c = getchar_unlocked()) != EOF; ) {
    if (c != '$') putchar_unlocked(c);
    else {
      char *failstring = NULL;
      size_t s = 0;
      FILE *fail = open_memstream(&failstring, &s);
      putc_unlocked('$', fail);

      char *var = getvar(stdin, fail);
      fclose(fail);
      if (var) fputs_unlocked(var, stdout);
      else fwrite(failstring, 1, s, stdout);
      free(failstring);
    }
  }
  return errno;
}
