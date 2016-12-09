#include "common.h"
char *lowerstr(char *s) { for (size_t i = 0; s[i]; i++) s[i] = tolower(s[i]); return s; }
char *upperstr(char *s) { for (size_t i = 0; s[i]; i++) s[i] = toupper(s[i]); return s; }
