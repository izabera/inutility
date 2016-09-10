#include "lib/common.h"

struct winsize w;
int main(int argc, char *argv[]) {
  options("");
  if (!isatty(1)) return -1;

  int fd/*, searchdir = 1*/;
  ioctl(1, TIOCGWINSZ, &w);
  char *map, *screen = malloc(w.ws_row*w.ws_col), *cmd = NULL, *search = NULL;
  if (!screen) return errno;

  setvbuf(stdout, NULL, _IONBF, 0);
  printf("%s", smcup civis);
  struct termios term;
  tcgetattr(1, &term);
  tcsetattr(1, TCSADRAIN, &rawterm);
  for (int q = 1; q <= argc; q++) {
    if ((fd = open(argv[q], O_RDONLY)) == -1) continue;
    struct stat st;
    fstat(fd, &st);
    if ((map = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0))) {
      char key[10] = { 0 };
      ssize_t keysize = 0;
      for (printf("%s", cup); (keysize = read(0, &key, 1)) > 0; printf("%s", cup)) {
        for (ssize_t j = 0; j < i + w.ws_row - 1; j++) puts(line[j]);
        printf("%s%s (%zd/%zd)%s", rev el, argv[q], i+1, tot-w.ws_row+1, sgr0);
        switch (key[0]) {
          case 'e': case 'E': case '\5' : case 'j': case 'J': case '\n': case '\16': i++; break;
          case 'y': case 'Y': case '\31': case 'k': case 'K': case '\v': case '\20': i--; break;
          case 'f': case 'F': case '\6': case '\26': i += w.ws_row; break;
          case 'b': case 'B': case '\2':             i -= w.ws_row; break;
          case 'd': case 'D': case '\4' : i += w.ws_row / 2; break;
          case 'u': case 'U': case '\25': i -= w.ws_row / 2; break;
          case '>': case 'G': i = tot; break;
          case '<': case 'g': i = 0;   break;
#if 0
          case '/': printf("%s", cnorm); search = readaline_basic("/"); searchdir =  1; break;
          case '?': printf("%s", cnorm); search = readaline_basic("?"); searchdir = -1; break;
          case '!': printf("%s", cnorm); cmd = readaline_basic("!"); free(cmd); break;
#endif
          case 'q': goto out;
          case '\x1b': if (keysize == 3 && key[1] == '[') {  // arrows
                         if (key[2] == 'A') i--;
                         if (key[2] == 'B') i++;
                       }
        }
      }
out: munmap(map, st.st_size);
    }
    close(fd);
  }

  printf("%s", rmcup cnorm);
  return errno;
}
