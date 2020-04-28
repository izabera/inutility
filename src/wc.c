#include "lib/common.h"

struct Wc { size_t w, c, m, l; };
#define bufsize 4096

static void printwc(struct Wc count, const char *name) {
  if (flag('l')) printf("%7zu ", count.l);
  if (flag('w')) printf("%7zu ", count.w);
  if (flag('m')) printf("%7zu ", count.m);
  if (flag('c')) printf("%7zu ", count.c);
  if (name) printf("%s", name);
  putchar('\n');
}

static struct Wc wc(int fd) { /* counts both utf8 chars and bytes, assuming valid utf8 */
  size_t words = 0, chars = 0, bytes = 0, lines = 0;
  int inword = 0;
  unsigned char buffer[bufsize];
  ssize_t len;
  while ((len = read(fd, buffer, bufsize)) > 0) {
    for (int i = 0; i < len; i++) {
      switch (buffer[i]) {
        case '\n': lines++; /* fallthrough */
        case ' ': case '\t': case '\r': case '\f': case '\v':
          if (inword) {
            inword = 0;
            words++;
          }
          break;
        default: inword = 1;
      }
      chars += (buffer[i] < 0x80 || buffer[i] > 0xBF);
    }
    bytes += len;
  }
  return (struct Wc){ words, bytes, chars, lines };
}

int main(int argc, char *argv[]) {
  options("clmw");
  int fl = (!!flag('w')) | (!!flag('c'))<<1 | flag('l')<<2 | (!!flag('m'))<<3;
  int saverrno, file = 0, stdinonce = 0;
  FILE *fileptr = stdin;
  if (!fl) {
    fl = 1|2|4;
    flag('c') = 1;
    flag('l') = 1;
    flag('w') = 1;
  }
  struct Wc count = { 0, 0, 0, 0 }, tot = { 0, 0, 0, 0 };

  if (argc == 1)
    goto inner;
  while (*++argv) {
         if (argv[0][0] == '-' && argv[0][1] == 0) { file = 0; fileptr = stdin; }
    else if ((file = open(argv[0], O_RDONLY)) == -1) continue;
    else fileptr = fdopen(file, "r");
inner:
    if (fileptr == stdin && stdinonce++) {
      count = (struct Wc) { 0, 0, 0, 0 };
      goto readdone;
    }
    struct stat st;
    saverrno = errno;
    if (fstat(file, &st) == -1 || !S_ISREG(st.st_mode) || st.st_size == 0) goto readit;
    if (fl == 2) {
      count.c = st.st_size;
      goto readdone;
    }
    if (fl == 6 || fl == 4) { // it's worth to special case this
      if (fl == 6) count.c = st.st_size;
      char buffer[bufsize], *p;
      ssize_t len, left;
      while ((len = read(file, buffer, bufsize)) > 0) {
        for (p = buffer, left = len; left > 0 && (p = memchr(p, '\n', left)); p++) {
          left = buffer + len - p;
          count.l++;
        }
      }
      goto readdone;
    }
    else {
readit:
      errno = saverrno;
      count = wc(file);
readdone:
      tot.c += count.c;
      tot.l += count.l;
      tot.m += count.m;
      tot.w += count.w;
      printwc(count, argc == 1 ? NULL : *argv);
    }
    if (fileptr != stdin) close(file);
  }
  if (argc > 2) printwc(tot, "total");
  return errno;
}
