#include "lib/common.h"

// all the tests must return the opposite value... so confusing...
static int bin(char *argv[2]) {
  if (!strcmp(argv[2], "=")) return !!strcmp(argv[1], argv[3]);
  if (!strcmp(argv[2], "!=")) return !strcmp(argv[1], argv[3]);
  if (!strcmp(argv[2], "<")) return !(strcmp(argv[1], argv[3]) < 0);
  if (!strcmp(argv[2], ">")) return !(strcmp(argv[1], argv[3]) > 0);
  int a = atoi(argv[1]), b = atoi(argv[3]);
  if (!strcmp(argv[2], "-ne")) return a != b;
  if (!strcmp(argv[2], "-eq")) return a == b;
  if (!strcmp(argv[2], "-le")) return a >  b;
  if (!strcmp(argv[2], "-lt")) return a >= b;
  if (!strcmp(argv[2], "-ge")) return a <  b;
  if (!strcmp(argv[2], "-gt")) return a <= b;
  struct stat st1, st2;
  if (stat(argv[1], &st1) == -1 || stat(argv[3], &st2) == -1) return 1;
  if (!strcmp(argv[2], "-nt")) return st1.st_mtime <= st2.st_mtime;
  if (!strcmp(argv[2], "-ot")) return st1.st_mtime >= st2.st_mtime;
  if (!strcmp(argv[2], "-ef")) return st1.st_dev != st2.st_dev || st1.st_ino != st2.st_ino;
  return 255;
}

static int un(char *argv[]) {
  if (!strcmp(argv[1], "-n")) return argv[1][0] == 0;
  if (!strcmp(argv[1], "-z")) return argv[1][0] != 0;
  if (!strcmp(argv[1], "-a")) return !!access(argv[2], F_OK);
  if (!strcmp(argv[1], "-e")) return !!access(argv[2], F_OK);
  if (!strcmp(argv[1], "-r")) return !!access(argv[2], R_OK);
  if (!strcmp(argv[1], "-w")) return !!access(argv[2], W_OK);
  if (!strcmp(argv[1], "-x")) return !!access(argv[2], X_OK);
  if (!strcmp(argv[1], "-t")) return !isatty(atoi(argv[2]));
  struct stat st;
  if (stat(argv[2], &st) == -1) return 1;
  if (!strcmp(argv[1], "-b")) return !S_ISBLK(st.st_mode);
  if (!strcmp(argv[1], "-c")) return !S_ISCHR(st.st_mode);
  if (!strcmp(argv[1], "-d")) return !S_ISDIR(st.st_mode);
  if (!strcmp(argv[1], "-f")) return !S_ISREG(st.st_mode);
  if (!strcmp(argv[1], "-h")) return !S_ISLNK(st.st_mode);
  if (!strcmp(argv[1], "-L")) return !S_ISLNK(st.st_mode);
  if (!strcmp(argv[1], "-p")) return !S_ISFIFO(st.st_mode);
  if (!strcmp(argv[1], "-S")) return !S_ISSOCK(st.st_mode);
  if (!strcmp(argv[1], "-s")) return st.st_size == 0;
  if (!strcmp(argv[1], "-u")) return !(S_ISUID & st.st_mode);
  if (!strcmp(argv[1], "-g")) return !(S_ISGID & st.st_mode);
  if (!strcmp(argv[1], "-k")) return !(S_ISVTX & st.st_mode);
  if (!strcmp(argv[1], "-N")) return !(st.st_atime <= st.st_mtime);
  if (!strcmp(argv[1], "-O")) return !(st.st_uid == geteuid());
  if (!strcmp(argv[1], "-G")) return !(st.st_gid == getegid());
  return 255;
}

static int test(int argc, char *argv[]) {
  int i;
  switch (argc) {
    default: return 255;
    case 0: return 1;
    case 1: return argv[1][0] == 0;
    case 2: return !strcmp(argv[1], "!") ? !test(argc-1, argv+1) : un(argv);
    case 3: return (i = bin(argv)) != 255 ? i :
                   !strcmp(argv[1], "!") ? !test(argc-1, argv+1) : 255;
    case 4: return !strcmp(argv[1], "!") ? !test(argc-1, argv+1) : 255;
  }
}

int main(int argc, char *argv[]) {
  if (!strcmp(argv[0], "["))
    if (strcmp(argv[--argc], "]")) return 255;

  return test(--argc, argv);
}
