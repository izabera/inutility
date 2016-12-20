#include "lib/common.h"

int main(int argc, char *argv[]) {
  options("");

  struct {
    struct str *line;
    size_t used, size;
  } lines;
  lines.size = 64;
  lines.line = malloc(lines.size * sizeof(struct str));

  int file = 0;
  FILE *fileptr = stdin;
  struct stat st;
  char *line = NULL, *map;
  size_t len = 0;
  ssize_t read;
  if (argc == 1)
    goto inner;

  while (*++argv) {
         if (argv[0][0] == '-' && argv[0][1] == 0) { file = 0; fileptr = stdin; }
    else if ((file = open(argv[0], O_RDONLY)) == -1) continue;
    else fileptr = fdopen(file, "r");
inner:
    fstat(file, &st);
    /* use mmap to avoid all the allocations for regular files */
    if (st.st_size > 0 &&
        (map = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, file, 0)) != (void*) -1) {
      char *end = map + st.st_size, *begin;
      /*printf("%zu\n", end - map);*/
      while ((begin = memrchr(end, '\n', end - map))) {
        putchar('x');
        /*write(1, begin, end - begin);*/
        /*fwrite(begin, 1, end - begin, stdout);*/
        end = begin - 1;
      }
      putchar('y');
      /*write(1, map, end - map);*/
      /*fwrite(map, 1, end - map, stdout);*/
      munmap(map, st.st_size);
    }
    else {
      lines.used = 0;
      while ((read = getline(&line, &len, fileptr)) != -1) {
        if (lines.used == lines.size) {
          lines.size *= 2;
          lines.line = realloc(lines.line, lines.size * sizeof(struct str));
        }
        lines.line[lines.used].str = malloc(read);
        lines.line[lines.used].len = read;
        memcpy(lines.line[lines.used++].str, line, read);
      }
      while (lines.used--) {
        fwrite_unlocked(lines.line[lines.used].str, 1, lines.line[lines.used].len, stdout);
        free(lines.line[lines.used].str);
      }
    }
    if (fileptr != stdin) fclose(fileptr);
  }
  return errno;
}
