#include "lib/common.h"

#define printline() do {                                                            \
                      if (((flag('d')|flag('D')) && count != 1) ||                  \
                          (flag('u') && count == 1) ||                              \
                          (!(flag('d')|flag('D')) && !flag('u'))) {                 \
                        if (flag('c')) printf("%7zu %s%c", count, prevline, delim); \
                        else printf("%s%c", prevline, delim);                       \
                      }                                                             \
                    } while (0)
int main(int argc, char *argv[]) {
  options("cdDiuzs|w|", .arglessthan = 3);
  if (argc >= 2) if (!(freopen(argv[1], "r", stdin))) return errno;
  if (argc == 3) if (!(freopen(argv[2], "w", stdout))) return errno;
  if (flag('s')) flag('s') = lastnum('s');
  if (flag('w')) flag('w') = lastnum('w');

  char *line = NULL, *prevline = NULL, same = 0, delim = !flag('z') * '\n';
  size_t len = 0, prevlen = 0, count = 1;
  ssize_t read;
  int (*compare)(const char *, const char *) = flag('i') ? strcasecmp : strcmp;
  int (*comparen)(const char *, const char *, size_t) = flag('i') ? strncasecmp : strncmp;
  if ((read = getdelim(&prevline, &len, !flag('z') * '\n', stdin)) <= 0) return 0; // no errno
  if (prevline[read-1] == '\n') prevline[read-1] = 0;
  while ((read = getline(&line, &len, stdin)) != -1) {
    if (line[read-1] == '\n') line[read-1] = 0;
    if (len < flag('s') || prevlen < flag('s')) same = 1;
    else if (flag('w') && flag('w') > flag('s'))
      same = !comparen(line+flag('s'), prevline+flag('s'), flag('w')-flag('s'));
    else same = !compare(line+flag('s'), prevline+flag('s'));
    if (same) { count++; if (flag('D')) printline(); }
    else { printline(); count = 1; }
    free(prevline);
    prevline = strdup(line);
  }
  printline();
  return errno;
}
