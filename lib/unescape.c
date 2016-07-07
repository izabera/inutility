#include "common.h"

int unescape(char *src, int *i) {
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
      character = src[*i] - '0';
      if (src[*i+1] >= '0' && src[*i+1] <= '9') {
        character = character * 8 + src[++*i] - '0';
        if (src[*i+1] >= '0' && src[*i+1] <= '9')
          character = character * 8 + src[++*i] - '0';
      }
  }
  return character;
}
