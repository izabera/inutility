#include "lib/common.h"

static void printstr(const char *str) {
  char *safe = "QWERTYUIOPASDFGHJKLZXCVBNM"
               "qwertyuiopasdfghjklzxcvbnm"
               "1234567890" "+-,./:=@_^%";
  if (strlen(str) == strspn(str, safe)) printf(" %s", str);
  else {
    printf(" '");
    for ( ; *str; str++) { 
      if (*str == '\'') printf("'\\''");
      else putchar_unlocked(*str);
    }
    putchar_unlocked('\'');
  }
}

static void noquotestr(const char *str) { printf(" %s", str); }

int main(int argc, char *argv[]) {
  options("ud:h:l|L|n:v:", .argleast = 1);
  struct opts opts = { 0 };
  char *name = "programname";
  void (*func)(const char *str) = printstr;
  // must free all the structures set by the previous options()
  if (flag('u')) {
    func = noquotestr;
    flag('u') = 0;
  }
  if (flag('d')) {
    opts.descr       = lastarg('d');
    flag('d') = 0;
    free(flags[opt('d')].args);
    flags[opt('d')].args = NULL;
  }
  if (flag('l')) {
    opts.argleast    = lastnum('l');
    flag('l') = 0;
    free(flags[opt('l')].args);
    flags[opt('l')].args = NULL;
  }
  if (flag('L')) {
    opts.arglessthan = lastnum('m');
    flag('L') = 0;
    free(flags[opt('L')].args);
    flags[opt('L')].args = NULL;
  }
  if (flag('n')) {
    name             = lastarg('n');
    flag('n') = 0;
    free(flags[opt('n')].args);
    flags[opt('n')].args = NULL;
  }
  if (flag('h')) {
    opts.help        = lastarg('h');
    flag('h') = 0;
    free(flags[opt('h')].args);
    flags[opt('h')].args = NULL;
  }
  if (flag('v')) {
    opts.version     = lastarg('v');
    flag('v') = 0;
    free(flags[opt('v')].args);
    flags[opt('v')].args = NULL;
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
         if (ptr[1] == ':')                  func(getarg(args));
    else if (ptr[1] == '*' || ptr[1] == '|') printf(" %" PRIi64, getarg(nums));
  }

  printf(" --");
  while (*++argv) func(*argv);
  putchar_unlocked('\n');
  return errno;
}
