#include "lib/common.h"

int main(int argc, char *argv[]) {
  options("", .argleast = 1);
  char *format = argv[1], *ptr, *currformat, doformat = 0;
  argv += 2;
  do {
    for (size_t i = 0; format[i]; i++) {
      switch (format[i]) {
        default: putchar_unlocked(format[i]); break;
        case '\\': putchar_unlocked(unescape(format, &i, 0)); break;
        case '%':
          if (format[i+1] == '%') { putchar_unlocked('%'); i++; break; }
          doformat = 1;
          int begin = i;
          while (strchr("#+-0123456789. ", format[++i])) ;
          currformat = strndup(format+begin, i-begin+1);
          switch (format[i]) {
            default: return -1;
            case 'A': case 'a': case 'E': case 'e': case 'F': case 'f': case 'G': case 'g':
              printf(currformat, *argv ? **argv == '\'' || **argv == '"' ? *++*argv++ : strtod(*argv++, NULL) : 0); break;
            case 'd': case 'i': case 'o': case 'u': case 'x': case 'X':
              printf(currformat, *argv ? **argv == '\'' || **argv == '"' ? *++*argv++ : strtoll(*argv++, NULL, 0) : 0);
              break;
            case 's':
              printf(currformat, *argv ? *argv++ : ""); break;
            case 'b':
              ptr = unescapestr(*argv ? *argv++ : "", 1);
              currformat[strlen(currformat)-1] = 's';
              printf(currformat, ptr);
              free(ptr);
              break;
            case 'c':
              putchar_unlocked(*argv ? **argv++ : '\0'); break;
          }
          free(currformat);
          break;
      }
    }
  } while (*argv && doformat);
  return errno;
}
