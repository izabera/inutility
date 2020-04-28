#include "lib/common.h"

int main(int argc, char *argv[]) {
  options("abcdilosvxA:j*N*t:w|");
  FILE *fileptr = stdin;

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
                default : i--; /* fallthrough */ // if -to we must go back to exit in the outer loop
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

  // space needed for printf
  int fieldwidth[9] = { 0 },
      addressw = 0,
      pad[] = {
        [d1] = 4, [d2] = 6, [d4] = 11, [d8] = 20,
        [o1] = 3, [o2] = 6, [o4] = 11, [o8] = 22,
        [u1] = 3, [u2] = 5, [u4] = 10, [u8] = 20,
        [x1] = 2, [x2] = 4, [x4] =  8, [x8] = 16,
      };

  size_t bufw = flag('w') ? lastnum('w') : 16, pos = 0;

  for (int i = 0; i < fmt.num; i++) {
    unsigned opt = fmt.list[i], type = opt >> 4, width = opt & 15;
    if (type < 4) { // if it's not -a or -c
      // sanity check for buffer width
      //      -w2 -tx4             -w3 -to2
      if (width > bufw || bufw % width) return 255;

      // -to1 -tx2
      // o1 is 3 char wide + 1 padding
      // x2 is shorter than two o1 fields, so fieldwidth[2] becomes 3*2+1
      /*switch (width) {*/
        /*case 1: */
      /*}*/
    }
    /*else fields[1] = 1;*/
  }

  // now adjust fieldwidth
  for (int i = 0; i < fmt.num; i++) {
    unsigned opt = fmt.list[i];
    if ((opt >> 4) < 4) { // if it's not -a or -c
      //      -w2 -tx4             -w3 -to2
      if ((opt & 15) > bufw || bufw % (opt & 15)) return 255;
    }
    /*else fields[1] = 1;*/
  }

  char *cur = aligned_alloc(8, bufw), // this will need to be wider for doubles
       *old = aligned_alloc(8, bufw);

  int64_t jcount = 0, jval = flag('j') ? lastnum('j') : 0,
          Ncount = 0, Nval = flag('N') ? lastnum('N') + jval : INT64_MAX;

  if (argc == 1) {
    argv[0] = "-";
    goto inner;
  }
  while (*++argv) {
         if (argv[0][0] == '-' && argv[0][1] == 0) fileptr = stdin;
    else if (!(fileptr = fopen(argv[0], "r"))) continue;
inner:
    do {
      if (pos == 0) {
        switch (addressfmt) {
          case 'd': addressw = printf("%07" PRIi64, Ncount); break;
          case 'o': addressw = printf("%07" PRIo64, Ncount); break;
          case 'x': addressw = printf("%06" PRIx64, Ncount); break;
        }
      }
      if (Ncount++ == Nval) break; // check before reading
      int ch = getc(fileptr);
      if (ch == EOF) break;
      if (jcount++ < jval) continue;
      cur[pos++] = ch;

      if (pos == bufw) {
        pos = 0;
        if (!flag('v') && !memcmp(cur, old, bufw)) puts("*");
        else {
          for (int i = 0; i < fmt.num; i++) {
            if (i && addressfmt != 'n') printf("%*s", addressw, "");
            unsigned opt = fmt.list[i];
            switch (opt) {
              case a:
                for (size_t j = 0; j < bufw; j++)
                  printf(" %*s", fieldwidth[1], namedchars_a[(unsigned char)cur[j]&~128]);
                break;
              case c:
                for (size_t j = 0; j < bufw; j++)
                  printf(" %*s", fieldwidth[1], namedchars_c[(unsigned char)cur[j]]);
                break;
              default:
                for (size_t j = 0; j < bufw; j += opt & 15) {
                  // handle -td specially because it's the only one that's signed
                  if (opt >> 4 == 0) {
                    int64_t value = 0; // = 0 because gcc is dumb
                    switch (opt & 15) {
                      case 1: value = *(int8_t *)(cur+j); break;
                      case 2: value = *(int16_t*)(cur+j); break;
                      case 4: value = *(int32_t*)(cur+j); break;
                      case 8: value = *(int64_t*)(cur+j); break;
                    }
                    printf(" %*" PRIi64, fieldwidth[opt & 15], value);
                  }
                  else {
                    uint64_t value = 0;
                    switch (opt & 15) {
                      case 1: value = *(uint8_t *)(cur+j); break;
                      case 2: value = *(uint16_t*)(cur+j); break;
                      case 4: value = *(uint32_t*)(cur+j); break;
                      case 8: value = *(uint64_t*)(cur+j); break;
                    }
                    char *format = opt >> 4 == 1 ? " %0*.*" PRIo64 :
                                   opt >> 4 == 2 ? " %*.*"  PRIu64 : " %0*.*" PRIx64;
                    printf(format, fieldwidth[opt & 15], pad[opt], value);
                  }
                }
            }
            putchar('\n');
          }
        }
        char *tmp = old;
        old = cur;
        cur = tmp;
      }
    } while (1);

    if (fileptr != stdin) fclose(fileptr);
  }
  return errno;
}
