#include "common.h"
void lowerstr(char *s) { for (size_t i = 0; s[i]; i++) s[i] = tolower(s[i]); }
void upperstr(char *s) { for (size_t i = 0; s[i]; i++) s[i] = toupper(s[i]); }
