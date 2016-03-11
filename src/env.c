#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "lib/flags.h"
extern char **environ;

int main(int argc, char *argv[]) {
  options("i0u:");
  if (flags[opt('i')].count)
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
