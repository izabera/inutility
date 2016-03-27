#include "common.h"

// target must be able to hold 32 bytes
// sets a bit for every character it finds
size_t bitmemspn(const char *s, size_t slen, const char *accept, size_t acceptlen, void *target) {
  char *targ = target;
  memset(target, 0, 32);
  if (slen == 0 || acceptlen == 0) return 0;

  unsigned char mask[32] = { 0 };
  size_t i;

  for (i = 0; i < acceptlen; i++)                    setbit(mask, accept[i]);
  for (i = 0; i < slen && checkbit(mask, s[i]); i++) setbit(targ, s[i]);
  return i;
}

// target must be able to hold targetsize bytes (valid sizes are 256*sizeof(uint8/16/32/64_t))
// increments a counter for every character it finds, up to targetsize/256
size_t sizememspn(const char *s, size_t slen, const char *accept, size_t acceptlen, void *target, size_t targetsize) {
  memset(target, 0, targetsize);
  if (slen == 0 || acceptlen == 0) return 0;

  unsigned char mask[32] = { 0 };
  size_t i;

  for (i = 0; i < acceptlen; i++)                    setbit(mask, accept[i]);
  for (i = 0; i < slen && checkbit(mask, s[i]); i++)
    switch (targetsize/256) {
      case sizeof(uint8_t ): ((uint8_t *)target)[(unsigned char)s[i]]++; break;
      case sizeof(uint16_t): ((uint16_t*)target)[(unsigned char)s[i]]++; break;
      case sizeof(uint32_t): ((uint32_t*)target)[(unsigned char)s[i]]++; break;
      case sizeof(uint64_t): ((uint64_t*)target)[(unsigned char)s[i]]++; break;
    }
  return i;
}
