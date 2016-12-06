#include "lib/common.h"

static int rm(const char *path, struct stat *st, int type, struct FTW *ftw) {
  if (flag('i')) {
    fprintf(stderr, "remove '%s'? ", path);
    char buf[10];
    fgets(buf, sizeof buf, stdin);
    if (tolower(buf[0]) != 'y') return 0;
  }
  if (ftw) { // recursive
    if (type & (FTW_D|FTW_DP|FTW_DNR)) rmdir(path);
    else unlink(path);
  }
  else {
    if (lstat(path, st) == -1) return errno;
    if (S_ISDIR(st->st_mode) && flag('d')) rmdir(path);
    else unlink(path);
  }
  if (flag('v') && !errno) printf("removed '%s'\n", path);
  return flag('f') ? 0 : errno;
}

int main(int argc, char *argv[]) {
  options("dfiRrv", .argleast = 1);
  if (flag('R')) flag('r') = 1;
  if (flag('r'))
    while (*++argv)
      nftw(*argv,
          (int (*)(const char *, const struct stat *, int, struct FTW *))rm,
          100, FTW_DEPTH|FTW_PHYS);
  else
    while (*++argv) rm(*argv, &(struct stat){ 0 }, 0, 0);
  return flag('f') ? 0 : errno;
}
