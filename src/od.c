#include "lib/common.h"

int main(int argc, char *argv[]) {
  options("bcdilosvxA:j*N*t:w|");
  FILE *fileptr = stdin;
  size_t address = 0;

  char namedchars[128][3] = {
#include "lib/namedchars.data"
  };

  if (!flag('w')) flag('w') = 16;

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
      a, c
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
              printf("%zu\n", i);
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

  char addressfmt = flag('A') ? lastarg('A')[0] : 0;
  /*size_t width = flag('w') ? lastnum('w') : 16, pos = 0;*/
  size_t pos = 0;
#define width 16
  // todo width (check -w2 -tx4 -to8)
  char *current = malloc(width), *old = malloc(width);


  if (argc == 1) {
    argv[0] = "-";
    goto inner;
  }
  while (*++argv) {
         if (argv[0][0] == '-' && argv[0][1] == 0) fileptr = stdin;
    else if (!(fileptr = fopen(argv[0], "r"))) continue;
inner:
    do {
      int ch = getc_unlocked(fileptr);
      if (ch == EOF) break;
      current[pos++] = ch;
      if (pos == width) {
        pos = 0;
        if (!flag('v') && !memcmp(current, old, width)) puts("*");
        else {
          for (int i = 0; i < fmt.num; i++) {
            switch (fmt.list[i]) {
              case a:
                for (int j = 0; j < width; j++)
                  printf(" %.3s", namedchars[(unsigned char)current[j]&~128]);
                break;
              case c:
                for (int j = 0; j < width; j++)
                  printf(isprint(current[j]) ? " %3c" :
                         current[j] == '\0' ? "  \\0": current[j] == '\a' ? "  \\a":
                         current[j] == '\b' ? "  \\b": current[j] == '\f' ? "  \\f":
                         current[j] == '\n' ? "  \\n": current[j] == '\r' ? "  \\r":
                         current[j] == '\t' ? "  \\t": current[j] == '\v' ? "  \\v":
                         " %03o", current[j] & 255);
                break;
              default:
                for (int j = 0; j < width; j += fmt.list[i] & 15) {
                  char *format = (fmt.list[i] >> 4)[(char *[]){ " %03d", " %03o", " %03u" " %02x" }];
                  // can't use a switch here...
                       if ((fmt.list[i] & 15) == C) printf(format, current[j]);
                  else if ((fmt.list[i] & 15) == S) printf(format, *(short*)(current+j) & (short)-1);
                  else if ((fmt.list[i] & 15) == I) printf(format, *(int  *)(current+j) & (int  )-1);
                  else if ((fmt.list[i] & 15) == L) printf(format, *(long *)(current+j) & (long )-1);
                }
            }
            /*int size = fmt.list[i] & 15;*/
            /*// temp buffer for printf*/
            /*char buf[size];*/
            /*memcpy(buf, current, size);*/
            /*for (int j = 0; j < width; j += size) {*/
            /*}*/
            putchar_unlocked('\n');
          }
        }
        char *tmp = old;
        old = current;
        current = tmp;
      }
    } while (1);

    if (fileptr != stdin) fclose(fileptr);
  }
  return errno;
}
