#include "lib/common.h"

int main() {
  htable *h = htable_new(10000);
  // shuf -r -n 1000000 /usr/share/dict/words > /tmp/dict
  // ./mycmd < /tmp/dict
  for (char buf[100]; fgets(buf, sizeof(buf), stdin); ) {
    *strchr(buf, '\n') = 0;
    htable_set(h, buf, buf+1);
  }
  puts(htable_get(h, "arrows"));
  return 0;
}
