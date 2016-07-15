#include "lib/common.h"

struct rangelist {
  struct rangelist *next;
  size_t first, last;
} ranges, *tmprange;

static void insertlist(size_t first, size_t last) {
  // insert in sorted order by first
  for (tmprange = &ranges; tmprange->next && tmprange->first < first; tmprange = tmprange->next) ;
  struct rangelist tmp = {
    .first = tmprange->first,
    .last = tmprange->last,
    .next = tmprange->next
  };
  tmprange->first = first;
  tmprange->last = last;
  tmprange->next = malloc(sizeof(*tmprange));
  tmprange->next->first = tmp.first;
  tmprange->next->last = tmp.last;
  tmprange->next->next = tmp.next;
}

int main(int argc, char *argv[]) {
  options("nszb:c:f:d:");
  FILE *fileptr = stdin;

  ssize_t read;
  size_t len;
  char *line;
  int ldelim = flag('z') ? '\0' : '\n', fdelim = flag('d') ? *lastarg('d') : '\t';
  if (!flag('b') && !flag('c') && !flag('f')) return -1;

  // todo: check that the list is valid
  char *list = flag('b') ? lastarg('b') : flag('c') ? lastarg('c') : lastarg('f');

  for (; *list; list++) {
    size_t first = 1, last = -1;
    char *tmp = list;
    if (*list != '-') first = strtoull(list, &tmp, 10);
    list = tmp;
    if (*list == 0)   { insertlist(first, first); break;    }
    if (*list == ',') { insertlist(first, first); continue; }
    list++; // *list == '-'
    if (*list == 0)   { insertlist(first, -1); break;    }
    if (*list == ',') { insertlist(first, -1); continue; }
    last = strtoull(list, &tmp, 10);
    insertlist(first, last);
    list = tmp;
    if (*list == 0) break;
  }

  tmprange = &ranges;
  while (tmprange->next && tmprange->next->next) { // now join them
    if (tmprange->last+1 >= tmprange->next->first) {
      tmprange->last = tmprange->last > tmprange->next->last ? tmprange->last : tmprange->next->last;
      struct rangelist *ptr = tmprange->next->next;
      free(tmprange->next);
      tmprange->next = ptr;
    }
    else tmprange = tmprange->next;
  }
  // this was hard

  if (argc == 1) {
    argv[0] = "-";
    goto inner;
  }
  while (*++argv) {
         if (argv[0][0] == '-' && argv[0][1] == 0) fileptr = stdin;
    else if (!(fileptr = fopen(argv[0], "r"))) continue;
inner:
    while ((read = getdelim(&line, &len, ldelim, fileptr)) > 0) {
      if (flag('b')) {
        for (tmprange = &ranges; tmprange->next && tmprange->first < (size_t) read; tmprange = tmprange->next)
          fwrite(line+tmprange->first-1, 1, tmprange->last-tmprange->first+1, stdout);
        if (line[read-1] == ldelim) putchar(line[read-1]);
      }
      else if (flag('c')) {
        char *tmp = line;
        size_t pos = 1;
        tmprange = &ranges;
        while (*tmp && tmprange->next) {
          if (pos < tmprange->first) {
            pos++;
            do { tmp++; } while ((*tmp & 0xC0) == 0x80);  // skip a full utf8 char
          }
          else if (pos == tmprange->first) {
            do {
              pos++;
              do { putchar(*tmp++); } while ((*tmp & 0xC0) == 0x80); // print a char
            } while (pos <= tmprange->last);
            tmprange = tmprange->next;
          }
        }
        if (line[read-1] == ldelim) putchar(line[read-1]);
      } // this was relatively easy
      else {
        char *next = memchr(line, fdelim, read), *current = line;
        if (!next) {
          if (!flag('s')) fwrite(line, 1, read, stdout);
          continue;
        }
        size_t field = 1, printdelim = 0;
        for (tmprange = &ranges; tmprange->next; tmprange = tmprange->next) {
          while (field < tmprange->first) {
            field++;
            if (!(current = next)) goto nextline;
            current++;
            next = memchr(next+1, fdelim, read-1-(next-line));   // check if off by 1
          }
          while (field <= tmprange->last) {
            if (printdelim++) putchar(fdelim);
            if (next) {
              fwrite(current, 1, next-current, stdout);
              current = next+1;
              next = memchr(next+1, fdelim, read-1-(next-line));
              field++;
            }
            else {
              fwrite(current, 1, read-(current-line)-(line[read-1] == ldelim), stdout);
              goto nextline;
            }
          }
        } // this ended up being more complex than i thought

nextline:
        if (line[read-1] == ldelim) putchar(line[read-1]);
      }
    }
    if (fileptr != stdin) fclose(fileptr);
  }
  return errno;
}
