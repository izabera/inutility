#include <string.h>
#include <libgen.h>
#include <stdio.h>

#include "src/proto.h"
int main_list(int, char **);

struct {
  char *name;
  int (*func)(int, char **);
} inutility[] = {
  { "list", main_list },
#include "src/struct.h"
};

#define arrsize(a) sizeof(a)/sizeof(*a)

int main_list(int argc, char *argv[]) {
  (void) argc; (void) argv;
  puts("inutility list:");
  for (size_t i = 0; i < arrsize(inutility); i++)
    puts(inutility[i].name);
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
