#include "lib/common.h"

static int fmtok(const char *str) {
  int f;
  for (f = 0; *str; str++) {
    if (str[0] == '%') {
      if (str[1] == '%') str++;
      else {
        while (str[1] && strchr("0123456789+-# .'", str[1])) str++;
        if (!str[1]) return 0;
        if (strchr("aAeEfFgG", str[1])) f++;
      }
    }
  }
  return f == 1;
}

static char *buildfmt(double val[3]) {
  char *buf, *ptr;
  size_t inte = 0, frac = 0;
  for (int i = 0; i < 3; i++) {
    asprintf(&buf, "%g", val[i]);
    if ((ptr = strchr(buf, '.'))) {
      frac = max(frac, strlen(ptr+1));
      *ptr = 0;
    }
    inte = max(inte, strlen(buf));
    free(buf);
  }
  asprintf(&buf, "%%0%zu.%zuf", inte + (frac ? frac + 1 : 0), frac);
  return buf;
}

int main(int argc, char *argv[]) {
  options("_ws:f:", .argleast = 1, .arglessthan = 4);
  double first = 1, last = 0 /* gcc */, incr = 1;
  switch (argc) {
    case 2: last  = strtod(argv[1], NULL);
            break;
    case 3: first = strtod(argv[1], NULL);
            last  = strtod(argv[2], NULL);
            break;
    case 4: first = strtod(argv[1], NULL);
            incr  = strtod(argv[2], NULL);
            last  = strtod(argv[3], NULL);
            break;
  }
  if ((incr < 0 && first < last) || (incr > 0 && first > last)) return 0;
  char *separator = flag('s') ? lastarg('s') : "\n",
       *format = flag('f') ? lastarg('f') :
                 flag('w') ? buildfmt((double[]){ first, last, incr }) : "%g";
  if (!fmtok(format)) return -1;
  double val = first;
#define cond ((incr > 0 && val <= last) || (incr < 0 && val >= last))
  if (cond) {
    printf(format, val);
    char *newfmt;
    asprintf(&newfmt, "%%s%s", format);
    if (!flag('f') && flag('w')) free(format);
    for (val += incr; cond; val += incr) printf(newfmt, separator, val);
    putchar_unlocked('\n');
  }
  return errno;
}
