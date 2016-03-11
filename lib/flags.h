struct flags {
  size_t count;
  char **args;
} flags[63];

#define isnum(x)   (x >= 48 && x <= 57)
#define isupper(x) (x >= 64 && x <= 90)
#define islower(x) (x >= 97 && x <= 122)
#define opt(x) (isnum(x)   ? x - 48 :   \
                isupper(x) ? x - 55 :   \
                islower(x) ? x - 61 : 63)

int optind, opterr;

int parseopts(int argc, char *argv[], const char *opts);

#define options(str) if (parseopts(argc-1, argv+1, str)) return 1; argc -= optind; argv += optind;

