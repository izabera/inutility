#include "lib/common.h"

int main(int argc, char *argv[]) {
  // sane option parsing, not that stupid thing posix requires
  options("abCefhimnsuvxc:o:");
  int exitcode = 0;
  FILE *fcode;
  char *code;
  size_t scode;
  if (flag('c')) {
    execute(lastarg('c'));
  else if (flag('s'))
    while 
  }
  return exitcode;
}
