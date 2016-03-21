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
                                                                                    
    case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7':
      character = src[*i] - '0';
      if (src[*i+1] >= '0' && src[*i+1] <= '9') {
        character = character * 8 + src[++*i] - '0';
        if (src[*i+1] >= '0' && src[*i+1] <= '9')
          character = character * 8 + src[++*i] - '0';
      }
  }
  return character;
}
