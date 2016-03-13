struct flags {
  size_t count;
  char **args;
};
extern struct flags flags[63];

#define isnum(x)   (x >= 48 && x <= 57)
#define isupper(x) (x >= 64 && x <= 90)
#define islower(x) (x >= 97 && x <= 122)
#define opt(x) (isnum(x)   ? x - 48 :   \
                isupper(x) ? x - 55 :   \
                islower(x) ? x - 61 : 63)

extern int parseoptind, parseopterr;

// https://github.com/mcinglis/c-style#use-structs-to-name-functions-optional-arguments
struct opts {
  char *shortopts, *descr, *help;
  short argleast, argnomore;
  /* todo: long opts */
};

int parseopts(int argc, char *argv[], const char *program, struct opts opts);

#define options(...) switch (parseopts(argc-1, argv+1, __func__,    \
                                  (struct opts) { __VA_ARGS__ })) { \
                       case 'h': return 0;                          \
                       case '?': case ':': return 1;                \
                     }                                              \
                     argc -= parseoptind;                           \
                     argv += parseoptind;

#define flag(x) flags[opt(x)].count
