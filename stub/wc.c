#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

struct Wc { size_t w, c, l; };
#define bufsize 4096

struct Wc wc(int fd) { /* counts both utf8 chars and bytes */
  size_t words = 0, chars = 0, bytes = 0, lines = 0;
  bool inword = false;
  unsigned char buffer[bufsize];
  ssize_t len;
  while ((len = read(fd, &buffer, bufsize)) > 0) {
    for (int i = 0; i < len; i++) {
      switch (buffer[i]) {
        case '\n': lines++;
        case ' ': case '\t': case '\r': case '\f': case '\v':
          if (inword) {
            inword = false;
            words++;
          }
          break;
        default: inword = true;
      }
      chars += (buffer[i] < 0x80 || buffer[i] > 0xBF);
    }
    bytes += len;
  }
  return (struct Wc){ words, chars, bytes, lines };
}

int main(int argc, char *argv[]) {
  options("wclm");
  int flags = !!flag('w') | !!(flag('c')||flag('m'))<<1 | !!flag('l')<<2;
  int saverrno;
  if (!flags) flags = 1|2|4;

  if (argc == 1)
    goto inner;
  while (*++argv) {
         if (argv[0][0] == '-' && argv[0][1] == 0) { file = 0; fileptr = stdin; }
    else if ((file = open(argv[0], O_RDONLY)) == -1) continue;
    else fileptr = fdopen(file, "r");
inner:
    saverrno = errno;
    if (flags == 2) {
    }
    else {
    }
    if (flag('l')) printf("%7d ", wc.l);
    if (flag('w')) printf("%7d ", wc.w);
    if (flag('c')) printf("%7d ", wc.c);
    if (name) printf("%s", name);
    putchar('\n');
  }
  else {
    bool print_total = (i+1 < argc);
    int fd;
    struct Wc count, tot = { 0 };
    for (; i < argc; i++) {
      if (argv[i][0] == '-' && argv[i][1] == '\0') {
        count = wc(0);
        print(count, argv[i], flags);
        tot.l += count.l; tot.w += count.w; tot.c += count.c; tot.m += count.m;
        continue;
      }
      if ((fd = open(argv[i], O_RDONLY)) == -1) continue;
      count = wc(fd);
      print(count, argv[i], flags);
      close(fd);
      tot.l += count.l; tot.w += count.w; tot.c += count.c; tot.m += count.m;
    }
    if (print_total) print(tot, "total", flags);
  }
  return errno;
}
