#include "lib/common.h"

// sort -R keeps equal lines together
// do a jenkins hash on the string starting with a random num
uint32_t randnum;
int (*cmpfun)(const char *, const char *);
static int strrand(const char *line1, const char *line2) {
  const char *p1 = line1, *p2 = line2;
  uint32_t hash1 = randnum, hash2 = randnum;
  char c;
  while ((c = *line1++)) { hash1 += c; hash1 += hash1 << 10; hash1 ^= hash1 >> 6; }
  while ((c = *line2++)) { hash2 += c; hash2 += hash2 << 10; hash2 ^= hash2 >> 6; }
  hash1 += (hash1 << 3); hash1 ^= (hash1 >> 11); hash1 += (hash1 << 15);
  hash2 += (hash2 << 3); hash2 ^= (hash2 >> 11); hash2 += (hash2 << 15);

  int ret = (hash1 > hash2) - (hash1 < hash2);
  return ret ? ret : strcmp(p1, p2);
}

static int numcmp(const char *line1, const char *line2) {
  return (int)(strtod(line1, NULL) - strtod(line2, NULL));
}

static int compare(const void *p1, const void *p2) {
  const char *line1 = *(char **)p1, *line2 = *(char **)p2;
  if (flag('b')) {
    while (isblank(*line1)) line1++;
    while (isblank(*line2)) line2++;
  }
  return (flag('r') ? -1 : 1) * cmpfun(line1, line2);
}

int main(int argc, char *argv[]) {
  options("bfnrRuVo:");
  if (flag('o'))
    if (!freopen(lastarg('o'), "w", stdout)) return errno;
  if (flag('R')) randnum = pcg32_random();
  cmpfun = flag('n') ? numcmp :
           flag('R') ? strrand :
           flag('V') ? strverscmp :
           flag('f') ? strcasecmp : strcmp;

  size_t len = 0, count = 0, arrsize = 128;
  char *line = NULL, **lines = malloc(128 * sizeof(char *));
  ssize_t read;
  FILE *fileptr = stdin;
  if (argc == 1)
    goto inner;

  while (*++argv) {
    if (argv[0][0] == '-' && argv[0][1] == 0) fileptr = stdin;
    else if (!(fileptr = fopen(argv[0], "r"))) continue;

inner:
    while ((read = getline(&line, &len, fileptr)) > 0) {
      if (line[read-1] == '\n') line[read-1] = 0;
      lines[count++] = strdup(line);
      if (count > arrsize) {
        arrsize *= 2;
        if (!(lines = realloc(lines, arrsize * sizeof(char *)))) return 1;
      }
    }
    if (fileptr != stdin) fclose(fileptr);
  }

  qsort(lines, count, sizeof(char *), compare);

  for (size_t i = 0; i < count; i++) {
    if (!flag('u') || i == 0 || compare(&line[i-1], &line[i])) {
      lines[i][len = strlen(lines[i])] = '\n';
      fwrite_unlocked(lines[i], len+1, 1, stdout);
    }
  }
  return errno;
}
