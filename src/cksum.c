#include "lib/common.h"

int main(int argc, char *argv[]) {
  options("");
  FILE *fileptr = stdin;
  if (argc == 1) {
    argv[0] = "-";
    goto inner;
  }
  while (*++argv) {
         if (argv[0][0] == '-' && argv[0][1] == 0) fileptr = stdin;
    else if (!(fileptr = fopen(argv[0], "r"))) continue;
inner: ;
    uint32_t sum = 0;
#include "lib/crc32.data"

    size_t size = 0;
    for (int c; (c = fgetc(fileptr)) != EOF; size++)
      sum = crc32table[(sum >> 24) ^ c] ^ (sum << 8);

    for (size_t c = size; c; c >>= 8)
      sum = crc32table[(sum >> 24) ^ (c & 0xff)] ^ (sum << 8);

    if (argc == 1) printf("%" PRIu32 " %zu\n", ~sum, size);
    else printf("%" PRIu32 " %zu %s\n", ~sum, size, *argv);

    if (fileptr != stdin) fclose(fileptr);
  }
  return errno;
}
