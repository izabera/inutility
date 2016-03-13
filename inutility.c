#include <string.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>

#include "src/proto.h"
int main_list(int, char **);

struct inutility {
  char *name;
  int (*func)(int, char **);
} inutility[] = {
  { "list", main_list },
#include "src/struct.h"
};

#include "lib/flags.h"

#define arrsize(a) sizeof(a)/sizeof(*a)

int cmp(const void *a, const void *b) {
  const struct inutility *inutila = a, *inutilb = b;
  return strcmp(inutila->name, inutilb->name);
}

int main_list(int argc, char *argv[]) {
  options("n", .arglessthan = 1);
  qsort(inutility, arrsize(inutility), sizeof(*inutility), cmp);
  size_t linelen = 0, i;
  if (arrsize(inutility)) {
    if (flag('n')) {
      for (i = 0; i < arrsize(inutility); i++)
        printf("%s\n", inutility[i].name);
    }
    else {
      puts("inutility list:");
      for (i = 0; i < arrsize(inutility); i++) {
        if ((linelen += strlen(inutility[i].name)) <= 80)
          printf("%s", inutility[i].name);
        else {
          printf("\n%s", inutility[i].name);
          linelen = strlen(inutility[i].name);
        }
        linelen++;
        putchar(' ');
      }
      if (linelen) putchar('\n');
    }
  }
  return 0;
}

int main(int argc, char *argv[]) {
  if (!strcmp(basename(argv[0]), "inutility")) {
    if (argc == 1)
      return main_list(argc, argv);
    argc--;
    argv++;
  }
  for (size_t i = 0; i < arrsize(inutility); i++)
    if (!strcmp(basename(argv[0]), inutility[i].name))
      return inutility[i].func(argc, argv);
  return 1;
}
