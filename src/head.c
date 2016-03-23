#include "lib/common.h"

int main(int argc, char **argv) {
  options("#n|c*qvz");
  int64_t number = flag('n') ? flags[opt('n')].nums[flag('n')-1] :
               flag('#') ? flags[opt('#')].nums[flag('#')-1] :
               flag('c') ? flags[opt('c')].nums[flag('c')-1] : 10;

  FILE *fileptr = stdin;
  int firstfile = 0, verbose = flag('v') || (argc > 2 && !flag('q'));
  char *line = NULL;

  char *bytebuffer;
  struct linebuffer { char *line; size_t len; } *linebuffer;
  if (number < 0) {
    if (flag('c')) {
      if (!(bytebuffer = malloc(BUFSIZ +                    -number))) return 1;
    }
    else {
      if (!(linebuffer = malloc(sizeof(struct linebuffer) * -number))) return 1;
    }
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
    if (verbose) printf("%s==> %s <==\n", firstfile++ ? "\n" : "", argv[0]);
    if (flag('c')) {
      if (number < 0) {
        char c; /* todo: replace this with something faster */
        for (i = 0; i > number; i--)
          if ((bytebuffer[-i] = getc(fileptr)) == EOF) goto nextfile;
        while ((c = getc_unlocked(fileptr)) != EOF) {
          putchar_unlocked(bytebuffer[-i%number]);
          bytebuffer[-i%number] = c;
          i--;
        }
      }
      else {
        char buf[BUFSIZ];
        i = number;
        while (i > BUFSIZ) {
          if ((read = fread(buf, 1, BUFSIZ, fileptr)) > 0)
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
        for (int64_t i = 0; i < number; i++) {
          if ((read = getdelim(&line, &len, !flag('z') * '\n', fileptr)) > 0)
            fwrite(line, 1, read, stdout);
          else break;
        }
      }
    }
nextfile:
    if (fileptr != stdin) fclose(fileptr);
  }
  return errno;
}
