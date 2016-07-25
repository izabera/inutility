#include "lib/common.h"

static int fmtok(const char *str) {
  int f;
  for (f = 0; *str; str++) {
    if (str[0] != '%') continue;
    if (str[1] == '%') str++;
    else {
      str += strspn(str+1, "0123456789+-# .'");
      if (!str[1]) return 0;
      if (strchr("aAeEfFgG", str[1])) f++;
    }
  }
  return f == 1;
}

size_t inte, frac;
static char *buildfmt(double val[3]) {
  char *buf, tmp[512];
  for (int i = 0; i < 3; i++) {
    snprintf(tmp, sizeof(tmp), "%lld", (long long)val[i]);
    inte = max(inte, strlen(tmp));
  }
  asprintf(&buf, "%%0%zu.%zuf", inte + (frac ? frac + 1 : 0), frac);
  return buf;
}

int main(int argc, char *argv[]) {
  options("_ws:f:", .argleast = 1, .arglessthan = 4);
  double first = 1, last = 0 /* gcc */, incr = 1;
  char *ptr;
  switch (argc) {
    case 2: last  = strtod(argv[1], NULL);
            if ((ptr = strchr(argv[1], '.'))) frac = max(frac, strlen(ptr+1));
            break;
    case 3: first = strtod(argv[1], NULL);
            if ((ptr = strchr(argv[1], '.'))) frac = max(frac, strlen(ptr+1));
            last  = strtod(argv[2], NULL);
            if ((ptr = strchr(argv[2], '.'))) frac = max(frac, strlen(ptr+1));
            break;
    case 4: first = strtod(argv[1], NULL);
            if ((ptr = strchr(argv[1], '.'))) frac = max(frac, strlen(ptr+1));
            incr  = strtod(argv[2], NULL);
            if ((ptr = strchr(argv[2], '.'))) frac = max(frac, strlen(ptr+1));
            last  = strtod(argv[3], NULL);
            if ((ptr = strchr(argv[3], '.'))) frac = max(frac, strlen(ptr+1));
            break;
  }
  if ((incr < 0 && first < last) || (incr > 0 && first > last)) return 0;
  char *separator = flag('s') ? lastarg('s') : "\n",
       *format = flag('f') ? lastarg('f') :
                 flag('w') ? buildfmt((double[]){ first, last, incr }) :
                             buildfmt((double[]){     0,    0,    0 }) ;
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
