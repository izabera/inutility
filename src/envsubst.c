#include "lib/common.h"

// parsing is hard

// parse a var after $
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
  fscanf(input, "%m[A-Za-z0-9_]", &var);
  if (!var) return NULL;
  fputs_unlocked(var, fail);

  if (braces) {
    if ((c = getchar_unlocked()) == EOF) return NULL;
    putc_unlocked(c, fail);
    if (c != '}') {
      ungetc(c, input);
      return NULL;
    }
  }
  return var;
}

int main(int argc, char *argv[]) {
  options("v", .arglessthan = 2);
  char *failstring = NULL, **vars = NULL;
  size_t s = 0, nvars = 0;

  if (flag('v') && argc == 1) return 255;
  if (argc == 2) {
    for (int i = 0; argv[1][i]; i++) {
      if (argv[1][i] == '$') {
        i++;
        FILE *fail = open_memstream(&failstring, &s),
             *str = fmemopen(argv[1]+i, strlen(argv[1]+i), "r");
        putc_unlocked('$', fail);

        char *var = getvar(str, fail);
        fclose(fail);
        if (var) { // add it to an array (todo: hash table?)
          vars = realloc(vars, ++nvars * sizeof(char*));
          vars[nvars-1] = var;
        }
        else i += s; // something that's not a var, ignore it and advance i
        free(failstring);
      }
    }
  }

  if (flag('v')) {
    for (size_t i = 0; i < nvars; i++) puts(vars[i]);
    return errno;
  }

  // same as above but now work on stdin instead of that string
  for (int c; (c = getchar_unlocked()) != EOF; ) {
    if (c != '$') putchar_unlocked(c);
    else {
      FILE *fail = open_memstream(&failstring, &s);
      putc_unlocked('$', fail);

      char *var = getvar(stdin, fail), *value;
      fclose(fail);

      if (var) { // found a var
        if (argc == 2) {
          for (size_t i = 0; i < nvars; i++)
            if (!strcmp(vars[i], var)) goto printvar; // var is whitelisted
          goto printfallback;
        }

printvar:
        value = getenv(var);
        if (value) fputs_unlocked(value, stdout);
      }
      else printfallback:
        fwrite(failstring, 1, s, stdout); // var not found or not whitelisted
      free(var);
      free(failstring);
    }
  }
  return errno;
}
