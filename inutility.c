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

#define arrsize(a) sizeof(a)/sizeof(*a)

int cmp(const void *a, const void *b) {
  const struct inutility *inutila = a, *inutilb = b;
  return strcmp(inutila->name, inutilb->name);
}

int main_list(int argc, char *argv[]) {
  (void) argc; (void) argv;
  puts("inutility list:");
  qsort(inutility, arrsize(inutility), sizeof(*inutility), cmp);
  char *separator = "";
  size_t linelen = 0;
  for (size_t i = 0; i < arrsize(inutility); i++) {
    if ((linelen += strlen(inutility[i].name)) > 80) {
      separator = "\n";
      linelen = 0;
    }
    printf("%s%s", separator, inutility[i].name);
    separator = " ";
  }
  putchar('\n');
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
