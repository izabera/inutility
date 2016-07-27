#include "lib/common.h"

int main(int argc, char *argv[]) {
  options(/*"C"*/ "fimnpuU", .argleast = 1);
  int fl = 0;
//if (flag('C')) fl |= CLONE_NEWCGROUP;
  if (flag('i')) fl |= CLONE_NEWIPC;
  if (flag('m')) fl |= CLONE_NEWNS;
  if (flag('n')) fl |= CLONE_NEWNET;
  if (flag('p')) fl |= CLONE_NEWPID;
  if (flag('u')) fl |= CLONE_NEWUTS;
  if (flag('U')) fl |= CLONE_NEWUSER;

  if (unshare(fl) == -1) return errno;

  argv++;
  if (!flag('f')) goto jumphere;
  switch (fork()) {
    case -1: return errno;
    case  0: jumphere:
             execvp(*argv, argv);
             return errno;
    default: ;
             int status;
             wait(&status);
  }
  return errno;
}
