#include "lib/common.h"

struct line {
  size_t lineno/*, start, len*/;
  char *line, *cmpstr;
};

// sort -R keeps equal lines together
// do a jenkins hash on the string starting with a random num
static uint32_t randnum;
static int (*cmpfun)(const char *, const char *);
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

static int human(const char *line1, const char *line2) {
  return (int)(parsenumb(line1) - parsenumb(line2));
}

static int simplenumcmp(const char *line1, const char *line2) {
  return (int)(strtod(line1, NULL) - strtod(line2, NULL));
}

static int monthsort(const char *line1, const char *line2) {
  int month1 = 0, month2 = 0;
  char *months = "janfebmaraprmayjunjulaugsepoctnovdec";
  for (int i = 1; i <= 12; i++) {
    if (strncasecmp(months, line1, 3) == 0) month1 = i;
    if (strncasecmp(months, line2, 3) == 0) month2 = i;
    months += 3;
  }
  return  month1 &&  month2 ? month1 - month2 :
         !month1 && !month2 ? 0 : month1 ? 1 : -1;
}

static int bignumcmp(const char *line1, const char *line2) {
  int neg1 = 1, neg2 = 1, cmp;
  if (*line1 == '-') line1++, neg1 = -1;
  if (*line2 == '-') line2++, neg2 = -1;
  if (neg1 == 1 && neg2 == -1) return  1; // -1 < 1
  if (neg1 == -1 && neg2 == 1) return -1;

  while (*line1 == '0') line1++; // skip zeros
  while (*line1 == '0') line2++;
  size_t len1 = strspn(line1, "1234567890"), len2 = strspn(line2, "1234567890");

  if (len1 != len2) return neg1 * ((len1 > len2) - (len1 < len2)); // long num > short num
  if ((cmp = strncmp(line1, line2, len1))) return neg1 * cmp; // simple number comparison

  line1 += len1 + (line1[len1] == '.');
  line2 += len1 + (line2[len1] == '.'); // skip integer part and .

  len1 = strspn(line1, "1234567890"); len2 = strspn(line2, "1234567890");
  len1 = len1 < len2 ? len1 : len2;
  if ((cmp = strncmp(line1, line2, len1))) return neg1 * cmp; // different prefixes

  line1 += len1; line2 += len1; // skip prefixes

  while (*line1 == '0') line1++;
  while (*line2 == '0') line2++; // skip zeros again

  return !*line1 && !*line2 ? 0 : isdigit(*line1) ? 1 : -1;
}

static int compare(const void *p1, const void *p2) {
  struct line *s1 = (struct line *)p1, *s2 = (struct line *)p2;
  char *str1 = s1->cmpstr, *str2 = s2->cmpstr;
  int c = cmpfun(str1, str2);
  if (!c && flag('s')) c = str1 > str2 ? 1 : -1;
  return flag('r') ? -c : c;
}


int main(int argc, char *argv[]) {
  options("bdfghinruMRVo:"); // todo z
  if (flag('o'))
    if (!freopen(lastarg('o'), "w", stdout)) return errno;
  if (flag('R')) randnum = pcg32_random();
  cmpfun = flag('n') ? bignumcmp :
           flag('g') ? simplenumcmp :
           flag('M') ? monthsort :
           flag('R') ? strrand :
           flag('R') ? human :
           flag('V') ? strverscmp :
           flag('f') ? strcasecmp : strcmp;
  int delim = /*flag('z') ? 0 :*/ '\n';

  size_t len = 0, count = 0, arrsize = 128;
  char *line = NULL;
  struct line *lines = malloc(128 * sizeof(struct line));

  ssize_t read;
  FILE *fileptr = stdin;
  if (argc == 1)
    goto inner;

  while (*++argv) {
    if (argv[0][0] == '-' && argv[0][1] == 0) fileptr = stdin;
    else if (!(fileptr = fopen(argv[0], "r"))) continue;

inner:
    while ((read = getdelim(&line, &len, delim, fileptr)) > 0) {
      if (line[read-1] == delim) line[read-1] = 0;
      lines[count].line = strdup(line);

      // preparse all the lines
      if (flag('b') || flag('d') || flag('i') || flag('k')) {
        size_t i;
        FILE *tmpfile = open_memstream(&lines[count].cmpstr, &i);
        for (i = 0; line[i]; i++)
          if (line[i] != '\t' && line[i] != ' ') break;
        for (     ; line[i]; i++)
          if (!((flag('d') && !(isblank(line[i]) || isalnum(line[i]))) ||
                (flag('i') && !isprint(line[i]))))
            putc_unlocked(line[i], tmpfile);
        fclose(tmpfile);
      }
      else lines[count].cmpstr = strdup(line);
      if (count++ > arrsize) {
        arrsize *= 2;
        if (!(lines = realloc(lines, arrsize * sizeof(struct line)))) return 1;
      }
    }
    if (fileptr != stdin) fclose(fileptr);
  }

  qsort(lines, count, sizeof(struct line), compare);

  for (size_t i = 0; i < count; i++) {
    if (!flag('u') || i == 0 || compare(&line[i-1], &line[i])) {
      lines[i].line[len = strlen(lines[i].line)] = '\n';
      fwrite_unlocked(lines[i].line, len+1, 1, stdout);
    }
  }
  return errno;
}