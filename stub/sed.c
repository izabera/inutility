#include "lib/common.h"

struct string {
  char *str;
  size_t size;
} patspace, holdspace;

size_t instr;

struct {
} *instructions;

void parselabel() {
  for (sedscript++; *sedscript == ' ' || *sedscript == '\t'; sedscript++) ;
  addlabel(
}

/*void parse*/
int main(int argc, char *argv[]) {
  options("inrsze:f:l:");
  char *template, *sedscript;
  size_t scriptlen;
  FILE *infile = stdin, *outfile = stdout, *fscript = open_memstream(&sedscript, &scriptlen), *tmpfile;

  if (flag('i')) flag('s') = 1;
  if (!flag('l')) flag('l') = 80;
  if (flag('e') || flag('f')) { // build the script
    size_t e = 0, f = 0;
    for ( ; *flaglist; flaglist++) {
      if (*flaglist == 'e') fprintf(fscript, "%s\n", flags[opt('e')].args[e++]);
      else if (*flaglist == 'f') {
        if (!(tmpfile = fopen(flags[opt('f')].args[f++], "r"))) return errno;
        int c;
        while ((c = fgetc(tmpfile)) != EOF) putc(c, fscript);
        fclose(tmpfile);
        putc('\n', fscript);
      }
    }
  }
  fclose(fscript);

  char *line;
  ssize_t read;
  size_t len, lineno = 1;
  int delim = flag('z') ? '\0' : '\n';

  if (argc == 1) {
    if ((!flag('f') && !flag('e')) || flag('i')) return -1;
    argv[0] = "-";
  }
  else {
    if (!flag('f') && !flag('e')) {
      free(sedscript);
      sedscript = *++argv;
      scriptlen = strlen(sedscript);
    }
  }

  parse(&sedscript);
  if (*sedscript) printf("trailing }\n"), return -1;
  if (!strncmp(sedscript, "#n")) flag('n') = 1;
  if (argc == 1) goto inner;

  while (*++argv) {
         if (argv[0][0] == '-' && argv[0][1] == 0) infile = stdin;
    else if (!(infile = fopen(argv[0], "r"))) continue;
inner:
    if (flag('s')) lineno = 1;
    if (flag('i')) {
      asprintf(&template, "%sXXXXXX", *argv);
      int tmpfd;
      if ((tmpfd = mkstemp(template)) == -1) return errno;
      fchmod(tmpfd, 0644);
      fclose(outfile);
      outfile = fdopen(tmpfd, "w");
    }
    while ((read = getdelim(&line, &len, delim, infile)) > 0) {
      lineno++;
      if (delayed) {
        puts(delayed);
        free(delayed);
        delayed = NULL;
      }
      for (instr = 0; instructions[instr]; instr++) {
        switch (instruction[instr]) {
          case 'a':
          case 'b':
        }
      }
      if (!flag('n')) {
        fwrite(patspace.str, patspace.size, 1, outfile);
        if (line[read-1] == delim) putc(delim, outfile);
      }
    }
    if (flag('i')) {
      if (rename(template, *argv) == -1) return errno;
      free(template);
    }
  }
  return errno;
}
