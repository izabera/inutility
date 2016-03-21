#include "lib/common.h"

int main(int argc, char *argv[]) {
  options("");

  struct {
    struct string {
      char *str;
      size_t size;
    } *line;
    size_t used, size;
  } lines;
  lines.size = 64;
  if (!(lines.line = malloc(lines.size * sizeof(struct string)))) return 1;

  int file = 0;
  FILE *fileptr = stdin;
  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  if (argc == 1)
    goto inner;

  /* todo: use mmap to avoid all these allocations for regular files */
  while (*++argv) {
         if (argv[0][0] == '-' && argv[0][1] == 0) { file = 0; fileptr = stdin; }
    else if ((file = open(argv[0], O_RDONLY)) == -1) continue;
    else fileptr = fdopen(file, "r");
inner:
      lines.used = 0;
      while ((read = getline(&line, &len, fileptr)) != -1) {
        if (lines.used == lines.size) {
          lines.size *= 2;
          if (!(lines.line = realloc(lines.line, lines.size * sizeof(struct string)))) return 1;
        }
        if (!(lines.line[lines.used].str = malloc(read))) return 1;
        lines.line[lines.used].size = read;
        memcpy(lines.line[lines.used++].str, line, read);
      }
      while (lines.used--) {
        fwrite(lines.line[lines.used].str, 1, lines.line[lines.used].size, stdout);
        free(lines.line[lines.used].str);
      }
    if (fileptr != stdin) fclose(fileptr);
  }
  return errno;
}
