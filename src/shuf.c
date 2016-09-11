#include "lib/common.h"

int main(int argc, char *argv[]) {
  options("erzi:n|");
  size_t maxlines = flag('n') ? lastnum('n') : -1, nelements = 0;
  int delim = flag('z') ? 0 : '\n';

  struct node {
    struct node *next;
    char *str;
    size_t size;
  } *head = calloc(1, sizeof(*head)), *last, *prev = head /* gcc */, *current;
  if (!head) return errno;
  last = head;

  if (flag('o')) if (!freopen(lastarg('o'), "w", stdout)) return errno;

#define addtolist(line, len)                            \
  do {                                                  \
    struct node *tmpnode = calloc(1, sizeof(*tmpnode)); \
    if (!tmpnode) return errno;                         \
    last->str = line;                                   \
    last->size = len;                                   \
    last->next = tmpnode;                               \
    prev = last;                                        \
    last = tmpnode;                                     \
    nelements++;                                        \
  } while (0)

  if (flag('e')) {
    while (*++argv) {
      char *tmp = strdup(*argv);
      if (!tmp) return errno;
      addtolist(tmp, strlen(tmp));
    }
  }
  else if (flag('i')) {
    if (lastarg('i')[0] == '-') return -1;
    // FIXME: this shit is temporary
    char *str;
    unsigned long long begin = strtoull(lastarg('i'), &str, 10);
    if (*str++ != '-') return -1;
    unsigned long long end   = strtoull(str, NULL, 10);
    for ( ; begin <= end; begin++) {
      if (asprintf(&str, "%llu", begin) == -1) return errno;
      addtolist(str, strlen(str));
    }
  }
  else {
    FILE *fileptr = stdin;
    ssize_t read;
    size_t len = 0;
    char *line = NULL;
    if (argc == 1) {
      argv[0] = "-";
      goto inner;
    }
    while (*++argv) {
           if (argv[0][0] == '-' && argv[0][1] == 0) fileptr = stdin;
      else if (!(fileptr = fopen(argv[0], "r"))) continue;
inner:
      while ((read = getdelim(&line, &len, delim, fileptr)) > 0) {
        char *tmp;
        if (line[read-1] == delim) read--;
        if (!(tmp = memdup(line, read))) return errno;
        addtolist(tmp, read);
      }
      if (fileptr != stdin) fclose(fileptr);
    }
  }

  free(last);
  if (nelements == 0) return errno;
  current = prev->next = head; // make it circular

  for (size_t i = 0; i < maxlines && nelements; i++) {
    uint64_t randn = ((uint64_t) pcg32_random() << 32 | pcg32_random()) % nelements;

    for (size_t j = 0; j < randn; j++) current = current->next;

    fwrite_unlocked(current->str, 1, current->size, stdout);
    putchar_unlocked(delim);

    if (!flag('r')) {
      free(current->str);
      current->str = current->next->str;
      current->size = current->next->size;
      struct node *tmp = current->next->next;
      free(current->next);
      current->next = tmp; // last element must check this?
      nelements--;
    }
  }
  return errno;
}
