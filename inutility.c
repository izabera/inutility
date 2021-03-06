#include "lib/common.h"

#include "src/proto.h"
static int main_list(int, char **);

static struct inutility {
  char *name;
  int (*func)(int, char **);
} inutility[] = {
  { "list", main_list },
#include "src/struct.h"
};

static int cmp(const void *a, const void *b) {
  const struct inutility *inutila = a, *inutilb = b;
  return strcmp(inutila->name, inutilb->name);
}

static int main_list(int argc, char *argv[]) {
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
        putchar_unlocked(' ');
      }
      if (linelen) putchar_unlocked('\n');
    }
  }
  return 0;
}

int main(int argc, char *argv[]) {
  if (!strcmp(basename(argv[0]), "inutility")) {
    if (argc == 1)
      return main_list(argc, argv);
#define __func__ "inutility"
    options("", .help = "[command] [args]");
    argc--;
    argv++;
  }
  for (size_t i = 0; i < arrsize(inutility); i++)
    if (!strcmp(basename(argv[0]), inutility[i].name))
      return inutility[i].func(argc, argv);
  return -1;
}
