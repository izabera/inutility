#include "lib/common.h"

int main(int argc, char *argv[]) {
  options("abcdilosvxA:j*N*t:w|");
  FILE *fileptr = stdin;
  size_t address = 0;

#include "lib/namedchars.data"

  // todo: float
#define C sizeof(char)
#define S sizeof(short)
#define I sizeof(int)
#define L sizeof(long)
  struct {
    enum {
      d1 =  0 | 1, d2 =  0 | 2, d4 =  0 | 4, d8 =  0 | 8,
      dC =  0 | C, dS =  0 | S, dI =  0 | I, dL =  0 | L, // with these,
      o1 = 16 | 1, o2 = 16 | 2, o4 = 16 | 4, o8 = 16 | 8,
      oC = 16 | C, oS = 16 | S, oI = 16 | I, oL = 16 | L, // the rest needs only
      u1 = 32 | 1, u2 = 32 | 2, u4 = 32 | 4, u8 = 32 | 8,
      uC = 32 | C, uS = 32 | S, uI = 32 | I, uL = 32 | L, // handle the cases
      x1 = 48 | 1, x2 = 48 | 2, x4 = 48 | 4, x8 = 48 | 8,
      xC = 48 | C, xS = 48 | S, xI = 48 | I, xL = 48 | L, // with numbers
      a = 100, c = 101
    } *list;
    int num;
  } fmt = { .list = NULL, .num = 0 };
#define addfmt(f) \
  do { \
    fmt.list = realloc(fmt.list, sizeof(fmt.list[0]) * ++fmt.num); \
    fmt.list[fmt.num-1] = f; \
  } while (0)

  int topt = 0;
  while (*flaglist) {
    switch (*flaglist++) {
      case 'a': addfmt( a); break;
      case 'b': addfmt(o1); break;
      case 'c': addfmt( c); break;
      case 'd': addfmt(u2); break;
      case 'i': addfmt(dI); break;
      case 'l': addfmt(dL); break;
      case 'o': addfmt(o2); break;
      case 's': addfmt(d2); break;
      case 'x': addfmt(x2); break;
      case 't':
        for (size_t i = 0; flags[opt('t')].args[topt][i]; i++) {
          switch (flags[opt('t')].args[topt][i]) {
            default: return 255;
            case 'a': addfmt(a); break;
            case 'c': addfmt(c); break;
            case 'd': case 'o': case 'u': case 'x': ;
              char *doux = "doux";
              int val = 16 * (strchr(doux, flags[opt('t')].args[topt][i++]) - doux);
              switch (flags[opt('t')].args[topt][i]) {
                case '1': val |= 1; break;
                case '2': val |= 2; break;
                case '4': val |= 4; break;
                case '8': val |= 8; break;
                case 'C': val |= C; break;
                case 'S': val |= S; break;
                default : i--; // if -to we must go back to exit in the outer loop
                case 'I': val |= I; break;
                case 'L': val |= L; break;
              }
              addfmt(val);
          }
        }
        topt++;
      }
  }
  if (!fmt.num) addfmt(oS);

  char addressfmt = flag('A') ? lastarg('A')[0] : 'o';
  if (!strchr("doxn", addressfmt)) return 255;

  size_t width = flag('w') ? lastnum('w') : 16, pos = 0;
  for (int i = 0; i < fmt.num; i++) {
    unsigned opt = fmt.list[i];
    if ((opt >> 4) < 4) { // if it's not -a or -c
      //      -w2 -tx4             -w3 -to2
      if ((opt & 15) > width || width % (opt & 15)) return 255;
    }
  }

  char *curr = malloc(width), *old = malloc(width);

  ssize_t jcount = 0, jval = flag('j') ? lastnum('j') : 0,
          Ncount = 0, Nval = flag('N') ? lastnum('N') + jval : -1;

  if (argc == 1) {
    argv[0] = "-";
    goto inner;
  }
  while (*++argv) {
         if (argv[0][0] == '-' && argv[0][1] == 0) fileptr = stdin;
    else if (!(fileptr = fopen(argv[0], "r"))) continue;
inner:
    do {
      if (Ncount++ == Nval) break; // check before reading
      int ch = getc_unlocked(fileptr);
      if (ch == EOF) break;
      if (jcount++ < jval) continue;
      curr[pos++] = ch;

      if (pos == width) {
        pos = 0;
        if (!flag('v') && !memcmp(curr, old, width)) puts("*");
        else {
          for (int i = 0; i < fmt.num; i++) {
            if (addressfmt)
            switch (fmt.list[i]) {
              case a:
                for (size_t j = 0; j < width; j++)
                  fwrite_unlocked(&namedchars_a[(unsigned char)curr[j]&~128], 1, 4, stdout);
                break;
              case c:
                for (size_t j = 0; j < width; j++)
                  fwrite_unlocked(&namedchars_c[(unsigned char)curr[j]], 1, 4, stdout);
                break;
#if 0
              default:
                for (size_t j = 0; j < width; j += fmt.list[i] & 15) {
                  char *format = (char *[]){ " %*d", " %0*o", " %0*u" " %0*x" }[(fmt.list[i] >> 4)];
                  size_t value = 0;
                  switch (fmt.list[i] & 15) {
                    case 1: value = 
                  }
                }
#endif
            }
            putchar_unlocked('\n');
          }
        }
        char *tmp = old;
        old = curr;
        curr = tmp;
      }
    } while (1);

    if (fileptr != stdin) fclose(fileptr);
  }
  return errno;
}
