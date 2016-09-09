//char *readaline(const char *);
char *readaline_basic(const char *);


// basic == backspace
// arrow == arrows backspace
// canon == ^U ^W backspace
// emacs == ^A ^E ^N ^P ^U ^W arrows backspace
// vi

struct readaline {
  enum { basic, arrow, canon, emacs, vi } mode;
  char **history;
};

extern struct termios rawterm;

// let's hope the current terminal likes xterm sequences
#define el    "\033[K"
#define rev   "\033[7m"
#define cup   "\033[1;1H"
#define sgr0  "\033(B\033[m"
#define rmcup "\033[?1049l"
#define smcup "\033[?1049h"
#define civis "\033[?25l"
#define cnorm "\033[?12l\033[?25h"
