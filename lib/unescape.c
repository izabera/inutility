#include "common.h"

int unescape(const char *src, size_t *i, int oct4) {
  int character;
  switch (src[++*i]) {
    case 'a':  character = '\a';    break;
    case 'b':  character = '\b';    break;
    case 'f':  character = '\f';    break;
    case 'n':  character = '\n';    break;
    case 'r':  character = '\r';    break;
    case 't':  character = '\t';    break;
    case 'v':  character = '\v';    break;
    case '\0':
    case '\\': character = '\\';    break;
    default:   character = src[*i]; break;

    case caseoct:
      if (!oct4) character = src[*i] - '0';
      else if (src[*i] == '0') character = 0;
      else { character = src[*i] - '0'; oct4 = 0; }
      if (src[*i+1] >= '0' && src[*i+1] <= '9') {
        character = character * 8 + src[++*i] - '0';
        if (src[*i+1] >= '0' && src[*i+1] <= '9') {
          character = character * 8 + src[++*i] - '0';
          if (oct4 && src[*i+1] >= '0' && src[*i+1] <= '9')
            character = character * 8 + src[++*i] - '0';
        }
      }
  }
  return character;
}

char *unescapestr(const char *src, int oct4) {
  size_t i = 0, siz;
  char *str;
  FILE *f = open_memstream(&str, &siz);
  while (src[i]) {
    putc_unlocked(src[i] == '\\' ? unescape(src, &i, oct4) : src[i], f);
    i++;
  }
  fclose(f);
  return str;
}
