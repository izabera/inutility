#include <stdint.h>
#include <stddef.h>

struct flags {
  size_t count;
  union {
    char **args;
    int64_t *nums;
  };
};
extern struct flags flags[64];
extern char *flaglist;

#define opt(x) ((x >= 48 && x <= 57) ? x - 48 :   \
                (x >= 64 && x <= 90) ? x - 55 :   \
                (x >= 97 && x <= 122)? x - 61 :   \
                x == '#'             ? 62     : 63)

extern int parseoptind, parseopterr;

// https://github.com/mcinglis/c-style#use-structs-to-name-functions-optional-arguments
struct opts {
  char *shortopts, *descr, *help, *version, *longopts;
  short argleast, arglessthan;
  /* todo: long opts */
};

int parseopts(int argc, char *argv[], const char *program, struct opts opts);

#define options(...) _Pragma("GCC diagnostic push")                                     \
                     _Pragma("GCC diagnostic ignored \"-Wmissing-field-initializers\"") \
                     switch (parseopts(argc-1, argv+1, __func__,                        \
                                  (struct opts) { __VA_ARGS__ })) {                     \
                     _Pragma("GCC diagnostic pop")                                      \
                       case 'h': case 'v': return 0;                                    \
                       case '?': case ':': case '@': case 'n': return -1;               \
                     }                                                                  \
                     argc -= parseoptind;                                               \
                     argv += parseoptind;

#define flag(x)    (flags[opt(x)].count)
#define lastarg(x) (flags[opt(x)].args[flag(x)-1])
#define lastnum(x) (flags[opt(x)].nums[flag(x)-1])
