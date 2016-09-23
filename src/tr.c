#include "lib/common.h"

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

static void makestr(struct str *dest, char *src) {
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
  for (size_t i = 0; src[i]; i++) {
    switch (src[i]) {
      case '[': for (size_t j = 0; j < arrsize(classes); j++) {
                  if (!strncmp(classes[j].name, src+i, classes[j].namelen)) {
                    fwrite_unlocked(classes[j].characters, 1, classes[j].charlen, stream);
                    i += classes[j].namelen - 1;
                    character = -1;
                    goto outfor;
                  }
                }
                fputc_unlocked(character = '[', stream);
                outfor: break;
      case '-': if (character == -1 || !src[i+1]) fputc_unlocked(character = '-', stream);
                else {
                  int from = character+1, to; /* from+1 because we already added that char */
                  if (src[++i] == '\\') to = unescape(src, &i, 0);
                  else to = src[i];
                  while (from <= to) fputc_unlocked(from++, stream);
                  character = -1;
                }
                break;
      case '\\': fputc_unlocked(character = unescape(src, &i, 0), stream);
                 break;
      default: fputc_unlocked(character = src[i], stream);
    }
  }
  fclose(stream);
  dest->len = bufsize;
}

int main(int argc, char *argv[]) {
  options("cCdst", .argleast = 1, .arglessthan = 3);
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
    str[0].str = (char *)tmp;
    str[0].len = j;
  }
  if (argc == 3) makestr(&str[1], argv[2]);

  for (i = 0; i < min(str[0].len, str[1].len); i++)
    map[(unsigned char)str[0].str[i]] = str[1].str[i];

  if (!flag('t') && argc == 3 && str[0].len > str[1].len)
    for (; i < str[0].len; i++)
      map[(unsigned char)str[0].str[i]] = str[1].str[(unsigned char)str[1].len-1];

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
  unsigned char inbuf[BUFSIZ], outbuf[BUFSIZ];
  int prev = -1;
  sequential(0);
  while ((read = fread_unlocked(inbuf, 1, sizeof(inbuf), stdin)) > 0) {
    for (i = j = 0; i < read; i++) {
      if (!delete[inbuf[i]]) {
        if (!squeeze[inbuf[i]] || prev != map[inbuf[i]])
          outbuf[j++] = prev = map[inbuf[i]];
      }
    }
    fwrite_unlocked(outbuf, 1, j, stdout);
  }

  return 0;
}
