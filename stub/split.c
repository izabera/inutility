#include "lib/common.h"

static inline void suffix(char *suff, size_t num) {
  printf("len: %zu\n", flag('a') ? lastnum('a') : 2);
  for (size_t i = 0; i < (flag('a') ? lastnum('a') : 2); i++) {
    printf("<%s>\n", suff);
    *(--suff) = flag('d') ? ('0' + num % 10) : ('a' + num % 26);
    num /= flag('d') ? 10 : 26;
  }
}

int main(int argc, char *argv[]) {
  options("deua|b*l:n:t:", .arglessthan = 3);
  char *prefix = argc == 2 ? "x" : argv[2],
       filename[strlen(prefix)+(flag('a') ? lastnum('a') : 2)+1];
  memcpy(filename, prefix, strlen(prefix)+1);
  printf("prefix: <%s>\n", prefix);
  FILE *infile = stdin, *outfile;
  if (argc > 1 && strcmp(argv[1], "-")) if (!(infile = fopen(argv[1], "r"))) return errno;

  size_t len, fileno = 0;
  ssize_t read;
  if (flag('b')) {
    char *buf = malloc(lastnum('b'));
    if (!buf) return errno;
    /*puts("X");*/
    puts(filename);
    suffix(filename+sizeof(filename)-1, fileno++);
    puts(filename);
    suffix(filename+sizeof(filename)-1, fileno++);
    puts(filename);
    /*while (fread_unlocked(buf, lastnum('b'), 1, infile)) {*/
      /*suffix(filename+sizeof(filename)-1, fileno++);*/

      /*if (!(outfile = fopen(filename, "w"))) return errno;*/
      /*fwrite_unlocked(buf, lastnum('b'), 1, outfile);*/
      /*fclose(outfile);*/
    /*}*/
  }
  /*while (read = get*/
  return errno;
}
