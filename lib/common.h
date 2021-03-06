#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stddef.h>
#include <stdint.h>
#include <inttypes.h>
#include <libgen.h>
#include <signal.h>
#include <time.h>
#include <limits.h>
#include <sys/sysinfo.h>
#include <utmpx.h>
#include <time.h>
#include <sys/time.h>
#include <alloca.h>
#include <sys/wait.h>
#include <sys/utsname.h>
#include <sys/vfs.h>
#include <grp.h>
#include <pwd.h>
#include <ctype.h>
#include <sched.h>
#include <dirent.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <wctype.h>
#include <wchar.h>
#include <locale.h>
#include <sys/sendfile.h>
#include <ftw.h>
#include <regex.h>

#include "flags.h"
#include "math.h"
//#include "memspn.h"
#include "parsenumb.h"
//#include "parsemode.h"
#include "random.h"
#include "unescape.h"
#include "uptime.h"
//#include "readaline.h"
#include "copyfd.h"
#include "hashtable.h"
#include "sequential.h"
#include "itoa.h"
#include "casestr.h"

#define arrsize(a) (sizeof(a)/sizeof(*a))
#define min(a,b) ((a) < (b) ? (a) : (b))
#define max(a,b) ((a) > (b) ? (a) : (b))

struct str {
  char *str;
  size_t len;
};

void *memdup(const void *mem, size_t len);
mode_t getumask();


// "gross" -- people in ##c
#define casebin        '0': case '1'
#define caseoct   casebin : case '2': case '3': case '4': case '5': case '6': case '7'
#define casedec   caseoct : case '8': case '9'
#define caselhex  casedec : case 'a': case 'b': case 'c': case 'd': case 'e': case 'f'
#define caseuhex  casedec : case 'A': case 'B': case 'C': case 'D': case 'E': case 'F'
#define casehex   caselhex: case 'A': case 'B': case 'C': case 'D': case 'E': case 'F'
#define caselower      'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': \
                            case 'h': case 'i': case 'j': case 'l': case 'm': case 'n': \
                            case 'o': case 'p': case 'q': case 'r': case 's': case 't': \
                            case 'u': case 'v': case 'w': case 'x': case 'y': case 'z'
#define caseupper      'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': \
                            case 'H': case 'I': case 'J': case 'L': case 'M': case 'N': \
                            case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': \
                            case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z'
#define casealpha caselower: case caseupper
#define casealnum casealpha: case casedec
#define casepunct     '\\': case '[': case ']': case '!': case '"': case '#': case '$': \
                            case '%': case '&': case '(': case ')': case '*': case '+': \
                            case ',': case '.': case '/': case ':': case ';': case '<': \
                            case '=': case '>': case '?': case '@': case '^': case '_': \
                            case '`': case '{': case '|': case '}': case '~': case '-': case '\''
#define caseblank      ' ': case '\t':
#define casespace caseblank: case '\v': case '\f': case '\r': case '\n'


// for _FORTIFY_SOURCE
#if __GNUC__ && ! __clang__
#define UNUSED(x) do { __auto_type __unused __attribute__((unused)) = (x); } while (0)
#else
#define UNUSED(x) (void) (x)
#endif


#define scalenum(x) ((x) > (1<<30) ? (x) / (1.0 * (1<<30)) : \
                     (x) > (1<<20) ? (x) / (1.0 * (1<<20)) : \
                     (x) > (1<<10) ? (x) / (1.0 * (1<<10)) : (x))
#define unit(x)     ((x) > (1<<30) ? 'G' : \
                     (x) > (1<<20) ? 'M' : \
                     (x) > (1<<10) ? 'K' : 'B')
#define scale(x) scalenum(x), unit(x)


#define Case    break; case
#define Default break; default

#define IBUFSIZ (128*1024)

#define attrpure __attribute__((pure))
#define attrconst __attribute__((const))

#define likely(x)   __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

#include "alloc.h"

// iterate over a compile time list
// iterate(string, "foo", "bar", "baz") {
//   puts(*string);
// }
#define iterate(var, firstelem, ...)                              \
  for (int __i = 0; __i < 1; )                                    \
  for (__auto_type __first = firstelem; __i < 1; __i++)           \
  for (__typeof(*&__first) __tmp[] = { firstelem, __VA_ARGS__ },  \
       *var = __tmp;                                              \
       var < __tmp + sizeof __tmp/sizeof *__tmp;                  \
       var++)

// int *ptr = new(int[10]);
#define new(x) malloc(sizeof(x))

#undef getc
#define getc getc_unlocked
#undef getchar
#define getchar getchar_unlocked
#undef putc
#define putc putc_unlocked
#undef putchar
#define putchar putchar_unlocked
#undef clearerr
#define clearerr clearerr_unlocked
#undef feof
#define feof feof_unlocked
#undef ferror
#define ferror ferror_unlocked
#undef fileno
#define fileno fileno_unlocked
#undef fflush
#define fflush fflush_unlocked
#undef fgetc
#define fgetc fgetc_unlocked
#undef fputc
#define fputc fputc_unlocked
#undef fread
#define fread fread_unlocked
#undef fwrite
#define fwrite fwrite_unlocked
#undef fgets
#define fgets fgets_unlocked
#undef fputs
#define fputs fputs_unlocked
