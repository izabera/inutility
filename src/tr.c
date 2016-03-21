#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "lib/flags.h"
#include "lib/common.h"

struct str {
  unsigned char *str;
  size_t len;
};

#define digit "0123456789"
#define upper "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define lower "abcdefghijklmnopqrstuvwxyz"
#define hex   "abcdefABCDEF"
#define hex0  "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f"
#define hex1  "\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f"
#define hex2  "\x20\x21\x22\x23\x24\x25\x26\x27\x28\x29\x2a\x2b\x2c\x2d\x2e\x2f"
#define hex2_     "\x21\x22\x23\x24\x25\x26\x27\x28\x29\x2a\x2b\x2c\x2d\x2e\x2f"
#define hex3  "\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x3a\x3b\x3c\x3d\x3e\x3f"
#define hex4  "\x40\x41\x42\x43\x44\x45\x46\x47\x48\x49\x4a\x4b\x4c\x4d\x4e\x4f"
#define hex5  "\x50\x51\x52\x53\x54\x55\x56\x57\x58\x59\x5a\x5b\x5c\x5d\x5e\x5f"
#define hex6  "\x60\x61\x62\x63\x64\x65\x66\x67\x68\x69\x6a\x6b\x6c\x6d\x6e\x6f"
#define hex7  "\x70\x71\x72\x73\x74\x75\x76\x77\x78\x79\x7a\x7b\x7c\x7d\x7e\x7f"
#define hex7_ "\x70\x71\x72\x73\x74\x75\x76\x77\x78\x79\x7a\x7b\x7c\x7d\x7e"
#define _hex7                                                             "\x7f"
#define blank " \t"
#define space " \f\n\r\t\v"
#define punct "[]!\"#$%&'()*+,./:;<=>?@\\^_`{|}~-"

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

void makestr(struct str *dest, char *src) {
  size_t bufsize;
  FILE *stream = open_memstream((char **)&(dest->str), &bufsize);
#define makeclass(name, chars) { name, chars, sizeof(name)-1, sizeof(chars)-1 },
  struct class {
    const char *name, *characters;
    size_t namelen, charlen;
  } classes[] = {
    makeclass("[:alnum:]",  upper lower digit)
    makeclass("[:alpha:]",  upper lower)
    makeclass("[:lower:]",  lower)
    makeclass("[:upper:]",  upper)
    makeclass("[:blank:]",  blank)
    makeclass("[:digit:]",  digit)
    makeclass("[:cntrl:]",  hex0 hex1 _hex7)
    makeclass("[:graph:]",  hex2_ hex3 hex4 hex5 hex6 hex7_)
    makeclass("[:print:]",  hex2  hex3 hex4 hex5 hex6 hex7_)
    makeclass("[:punct:]",  punct)
    makeclass("[:space:]",  space)
    makeclass("[:xdigit:]", digit hex)
  };

  int character = -1;
  for (int i = 0; src[i]; i++) {
    switch (src[i]) {
      case '[': for (size_t j = 0; j < arrsize(classes); j++) {
                  if (!strncmp(classes[j].name, src+i, classes[j].namelen)) {
                    fwrite(classes[j].characters, 1, classes[j].charlen, stream);
                    i += classes[j].namelen;
                    character = -1;
                    goto outfor;
                  }
                }
                fputc(character = '[', stream);
                outfor: break;
      case '-': if (character == -1 || !src[i+1]) fputc(character = '-', stream);
                else {
                  int from = character+1, to; /* from+1 because we already added that char */
                  if (src[++i] == '\\') to = unescape(src, &i);
                  else to = src[i];
                  while (from <= to) fputc(from++, stream);
                  character = -1;
                }
                break;
      case '\\': fputc(character = unescape(src, &i), stream);
                 break;
      default: fputc(character = src[i], stream);
    }
  }
  fclose(stream);
  dest->len = bufsize;
}

int main(int argc, char *argv[]) {
  options("cCtds", .argleast = 1, .arglessthan = 3);
  if (argc == 1 || argc > 3) return 1;
  if (argc == 2 && !(flag('d') || flag('s'))) return 1;
  if (argc == 3 && flag('d') && !flag('s')) return 1;
  if (flag('C')) flag('c') = 1;
  unsigned char map[256], delete[256] = { 0 }, squeeze[256] = { 0 }, tmp[256] = { 0 }; /* only supports LANG=C */
  size_t i, j;
  for (i = 0; i < 256; i++) map[i] = i;

  struct str str[2] = { { NULL, 0 }, { NULL, 0 } };

  makestr(&str[0], argv[1]);
  if (flag('c')) {
    for (i = j = 0; i < 256; i++)
      if (!memchr(str[0].str, i, str[0].len)) tmp[j++] = i;
    str[0].str = tmp;
    str[0].len = j;
  }
  if (argc == 3) makestr(&str[1], argv[2]);

  for (i = 0; i < min(str[0].len, str[1].len); i++)
    map[(int)str[0].str[i]] = str[1].str[i];

  if (!flag('t') && argc == 3 && str[0].len > str[1].len)
    for (; i < str[0].len; i++)
      map[(int)str[0].str[i]] = str[1].str[(int)str[1].len-1];

  if (flag('d')) {
    for (i = 0; i < 256; i++)
      if (memchr(str[0].str, i, str[0].len)) delete[i] = 1;
    if (flag('s')) {
      if (argc == 2) return 1;
      for (i = 0; i < 256; i++)
        if (memchr(str[1].str, i, str[1].len)) squeeze[i] = 1;
    }
  }
  else if (flag('s')) {
    for (i = 0; i < 256; i++)
      if (memchr(str[0].str, i, str[0].len)) squeeze[i] = 1;
  }


  size_t read;
  char inbuf[BUFSIZ], outbuf[BUFSIZ];
  int prev = -1;
  while ((read = fread(inbuf, 1, BUFSIZ, stdin)) > 0) {
    for (i = j = 0; i < read; i++) {
      if (!delete[(int)inbuf[i]]) {
        if (!squeeze[(int)inbuf[i]] || prev != map[(int)inbuf[i]])
          outbuf[j++] = prev = map[(int)inbuf[i]];
      }
    }
    fwrite(outbuf, 1, j, stdout);
  }

  return 0;
}
