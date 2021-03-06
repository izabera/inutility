#include "lib/common.h"

int main(int argc, char **argv) {
  options("qvz#c*n|");
  int64_t number = flag('n') ? flags[opt('n')].nums[flag('n')-1] :
                   flag('#') ? flags[opt('#')].nums[flag('#')-1] :
                   flag('c') ? flags[opt('c')].nums[flag('c')-1] : 10;

  FILE *fileptr = stdin;
  int firstfile = 0, verbose = flag('v') || (argc > 2 && !flag('q'));
  char *line = NULL;

  char *bytebuffer;
  struct linebuffer { char *line; size_t len; } *linebuffer;
  if (number < 0) {
    if (flag('c')) bytebuffer = malloc(BUFSIZ +                    -number);
    else           linebuffer = malloc(sizeof(struct linebuffer) * -number);
  }

  size_t len = 0;
  ssize_t read;
  int64_t i = 0;
  if (argc == 1) {
    argv[0] = "-";
    goto inner;
  }
  while (*++argv) {
         if (argv[0][0] == '-' && argv[0][1] == 0) fileptr = stdin;
    else if (!(fileptr = fopen(argv[0], "r"))) continue;
inner:
    sequential(fileno(fileptr));
    if (verbose) printf("%s==> %s <==\n", firstfile++ ? "\n" : "", argv[0]);
    if (flag('c')) {
      if (number < 0) {
        int c; /* todo: replace this with something faster */
        for (i = 0; i > number; i--)
          if ((c = getc(fileptr)) == EOF) goto nextfile;
          else bytebuffer[-i] = c;
        while ((c = getc(fileptr)) != EOF) {
          putchar(bytebuffer[-i%number]);
          bytebuffer[-i%number] = c;
          i--;
        }
      }
      else {
        char buf[BUFSIZ];
        i = number;
        while ((size_t)i > sizeof(buf)) {
          if ((read = fread(buf, 1, sizeof(buf), fileptr)) > 0)
            fwrite(buf, 1, read, stdout);
          else break;
          i -= read;
        }
        if ((read = fread(buf, 1, i, fileptr)) > 0)
          fwrite(buf, 1, read, stdout);
      }
    }
    else {
      if (number < 0) {
        for (i = 0; i > number; i--) {
          if ((read = getdelim(&line, &len, !flag('z') * '\n', fileptr)) <= 0) goto nextfile;
          linebuffer[-i].line = memdup(line, read);
          linebuffer[-i].len = read;
        }
        while ((read = getdelim(&line, &len, !flag('z') * '\n', fileptr)) > 0) {
          fwrite(linebuffer[-i%number].line, 1, linebuffer[-i%number].len, stdout);
          free(linebuffer[-i%number].line);
          linebuffer[-i%number].line = memdup(line, read);
          linebuffer[-i%number].len = read;
          i--;
        }
      }
      else {
        char buf[BUFSIZ];
        for (int64_t q = 0, j; q < number; ) {
          if ((read = fread(buf, 1, sizeof(buf), fileptr)) > 0) {
            for (j = 0; j < read && q < number; )
              if (buf[j++] == '\n') --number;
            fwrite(buf, 1, j, stdout);
          }
          else break;
        }
      }
    }
nextfile:
    if (fileptr != stdin) fclose(fileptr);
  }
  return errno;
}
