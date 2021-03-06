#include "lib/common.h"

static void printstr(const char *str, int shell) {
  char *safe = "QWERTYUIOPASDFGHJKLZXCVBNM"
               "qwertyuiopasdfghjklzxcvbnm"
               "1234567890" "+-,./:@_";
  if (strlen(str) == strspn(str, safe)) printf(" %s", str);
  else {
    printf(" '");
    for ( ; *str; str++) {
      switch (shell) {
        Case 0:
               if (*str == '\'') printf("'\\''" );
          else putchar(*str);
        Case 1: // not sure about this...
               if (*str == '\'') printf("'\\''" );
          else if (*str == '\\') printf("\\\\"  ); // especially this...
          else if (*str == '\n') printf("\\n"   );
          else if (*str == '!' ) printf("'\\!'" );
          else if (*str == ' ' ) printf("'\\ '" );
          else if (*str == '\t') printf("'\\\t'");
          else putchar(*str);
        Case 2: // or this...
               if (*str == '\'') printf("''"    );
          else putchar(*str);
        Case 3: // giucam says this is fine
               if (*str == '\'') printf("\\'"   );
          else if (*str == '\\') printf("\\\\"  );
          else putchar(*str);
      }
    }
    putchar('\'');
  }
}

static void noquotestr(const char *str, int shell) { (void) shell; printf(" %s", str); }

int main(int argc, char *argv[]) {
  char *optstring = "qQTud:h:l|L|n:s:v:", *name = argv[0];
  options(optstring, .argleast = 1);
  struct opts opts = { NULL, NULL, NULL, NULL, NULL, 0, 0 };
  void (*func)(const char*, int) = flaglist[0] ? printstr : noquotestr; // copy what others do
  int shell = 0; // posix sh

  if (flag('d')) opts.descr       = lastarg('d');
  if (flag('l')) opts.argleast    = lastnum('l');
  if (flag('L')) opts.arglessthan = lastnum('m');
  if (flag('n')) name             = lastarg('n');
  if (flag('h')) opts.help        = lastarg('h');
  if (flag('v')) opts.version     = lastarg('v');
  if (flag('q')) parseopterr      = 0;
  if (flag('Q')) if (!freopen("/dev/null", "w", stdout)) return errno;
  if (flag('T')) return 4; // pretend to be gnu getopt
  if (flag('s')) {
    func = printstr;
    char *  sh[] = { "sh", "ash", "bash", "dash", "ksh", "mksh", "pdksh", "posh", "yash", "zsh", NULL },
         * csh[] = { "csh", "tcsh", NULL },
         *  rc[] = { "rc", NULL },
         *fish[] = { "fish", NULL },  // fish support!
         **shells[] = { sh, csh, rc, fish };
    for (size_t i = 0; i < sizeof(shells)/sizeof(**shells); i++)
      for (size_t j = 0; shells[i][j]; j++)
        if (!strcmp(shells[i][j], lastarg('s'))) shell = i; // not found => keep sh
  }
  if (getenv("GETOPT_COMPATIBLE") || flag('u')) func = noquotestr;

  // must free all the structures set by the previous parseopts()
  for ( ; *optstring; optstring++) {
    if (!strchr("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789#", *optstring))
      continue;
    flag(*optstring) = 0;
    free(flags[opt(*optstring)].args);
    flags[opt(*optstring)].args = NULL;
  }
  free(flaglist);

  opts.shortopts = argv[1];
  switch (parseopts(argc-2, argv+2, name, opts)) {
    case 'h': return -'h';
    case 'v': return -'v';
    case '?': case ':': case '@': case 'n': return -1;
  }
  argv += parseoptind + 1;

  size_t idx[64] = { 0 };
#define getarg(type) (flags[opt(*flaglist)].type[idx[opt(*flaglist)]++])
  for ( ; *flaglist; flaglist++) {
    if (*flaglist == '#') {
      printf(" -%" PRIi64, getarg(nums));
      continue;
    }
    printf(" -%c", *flaglist);
    char *ptr = strchr(opts.shortopts, *flaglist);
         if (ptr[1] == ':')                  func(getarg(args), shell);
    else if (ptr[1] == '*' || ptr[1] == '|') printf(" %" PRIi64, getarg(nums));
  }

  printf(" --");
  while (*++argv) func(*argv, shell);
  putchar('\n');
  return errno;
}
