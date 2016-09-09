#include "common.h"

// readaline always returns a malloc'd string

static struct winsize w;
struct termios old, rawterm = {
  .c_iflag = ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL|IXON),
  .c_oflag = ~OPOST,
  .c_lflag = ~(ECHO|ECHONL|ICANON|ISIG|IEXTEN),
  .c_cflag = ~(CSIZE|PARENB),
  .c_cflag = CS8,
  .c_cc[VMIN] = 1,
  .c_cc[VTIME] = 0,
};

static void readaline_init() {
  setvbuf(stdout, NULL, _IONBF, 0);
  ioctl(1, TIOCGWINSZ, &w);
  tcgetattr(0, &old);
  tcsetattr(1, TCSADRAIN, &rawterm);
}

static void readaline_end() {
  /*setvbuf(stdout, NULL, _IOLBF, 0);*/  // this is annoying to restore and not that useful
  tcsetattr(1, TCSADRAIN, &old);
  printf("\r" el);
}

char *readaline_basic(const char *prompt) {
  readaline_init();
  size_t len = 0, p, columns = w.ws_col - 1 - strlen(prompt); // a char for the cursor
  wchar_t string[4096+columns], printthis[columns];
  memset(string, 0, sizeof(string));

  printf("%s", prompt);
  for (wint_t c; (c = getwchar_unlocked()) != WEOF; ) {
    switch (c) {
      case L'\b': case L'\177': string[len ? --len : 0] = 0; break;             // backspace
      case L'\r': case L'\n': goto out;
      default   : if (iswprint(c)) string[len++] = c; break;
    }
    memset(printthis, 0, sizeof(printthis));
    memcpy(printthis, string + len - ((len-1) % columns)-1, columns * sizeof(wchar_t));

    p = len ? (len % columns ? len % columns : len) : 0;
    printf("\r%s%ls" el "\r\033[%zuC", prompt, printthis, p + strlen(prompt));  // move cur
  }

out:
  readaline_end();
  char *ret = malloc(len = wcstombs(NULL, string, 0)+1);
  if (ret) wcstombs(ret, string, len);
  return ret;
}

#if 0

// basic == backspace
// arrow == arrows backspace
// canon == ^U ^W backspace
// emacs == ^A ^E ^N ^P ^U ^W arrows backspace

static char *readaline(const char *prompt) {
  setvbuf(stdout, NULL, _IONBF, 0);
  struct winsize w;
  ioctl(1, TIOCGWINSZ, &w);
  size_t len = 0, curpos = 0, columns = w.ws_col - 1 - strlen(prompt); // a char for the cursor
  wchar_t string[4096+columns], printthis[columns];
  memset(string, 0, sizeof(string));
  char buf[10] = { 0 };

  struct termios old, raw;
  tcgetattr(0, &old);
  cfmakeraw(&raw);
  tcsetattr(1, TCSADRAIN, &raw);

  printf("%s", prompt);
  for (ssize_t readlen; len < 4096 && (readlen = read(0, buf, sizeof(buf))) > 0; ) {
    if (buf[0] == '\x1b') {
      if (readlen != 3) continue;
      if (!memcmp("\x1b[C", buf, 3)) curpos += curpos < len - 1;
      if (!memcmp("\x1b[D", buf, 3)) curpos -= curpos > 0;
      continue;
    }
    wchar_t c;
    if (mbtowc(&c, buf, readlen) == -1) continue;
    switch (c) {
      case L'\b': case L'\177':                                                 // backspace
        if (curpos > 0) {
          memmove(string+curpos-1, string+curpos, sizeof(wchar_t) * (len - curpos + 1));
          len--; curpos--;
        }
        break;
      case L'\4': case L'\r': case L'\n': goto out;                             // ^D or enter
      /*case L'\27': while (len > 0 && string[len-1] == L' ') string[--len] = 0;  // ^W*/
                   /*while (len > 0 && string[len-1] != L' ') string[--len] = 0;*/
                   /*break;*/
      case L'\25': memset(string, 0, sizeof(wchar_t) * columns); len = curpos = 0; break;
      case L'\v': case L'\f': case L'\3': break;                                // ignore ^C
      default   : if (iswprint(c)) { curpos++; string[len++] = c; }; break;
    }
    memset(printthis, 0, sizeof(printthis));
    memcpy(printthis, string + len - ((len-1) % columns)-1, columns * sizeof(wchar_t));

    size_t p = curpos > 0 ? (curpos % columns ? curpos % columns : columns) : 0;
    printf("\r%s%ls\033[K\r\033[%zuC", prompt, printthis, p + strlen(prompt));      // \e[K == el
  }

out:
  printf("\r\033[K");
  setvbuf(stdout, NULL, _IOLBF, 0);
  tcsetattr(1, TCSADRAIN, &old);
  char *ret = malloc(len = wcstombs(NULL, string, 0)+1);
  if (ret) wcstombs(ret, string, len);
  return ret;
}

#endif
