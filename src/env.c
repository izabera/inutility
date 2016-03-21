#include "lib/common.h"

extern char **environ;

int main(int argc, char *argv[]) {
  options("i0u:");

  if (argv[1][0] == '-' && argv[1][1] == 0) {
    flag('i') = 1;
    argv++;
  }
  if (flag('i'))
    *environ = NULL;
  while (flags[opt('u')].count--)
    unsetenv(flags[opt('u')].args[flags[opt('u')].count]);

  char *var;
  while (*++argv) {
    if ((var = strchr(*argv, '='))) {
      *var = 0;
      setenv(*argv, var+1, 1);
    }
    else {
      execvp(*argv, argv);
      return 127;
    }
  }

  while (*environ)
    printf("%s%c", *environ++, flags[opt('0')].count ? 0 : '\n');

  return 0;
}
