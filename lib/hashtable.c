#include "common.h"

typedef struct htable {
  char **keys, **values;
  uint32_t size, used;
} htable;

static uint32_t fnv1a(const char *s) {
  uint32_t hash = 2166136261;
  for (; *s; s++) {
    hash ^= *s;
    hash *= 16777619;
  }
  return hash;
}

#define tombstone ((char *)-1)
static uint32_t find(htable *h, const char *key, int skiptombs) {
  for (uint32_t idx = fnv1a(key); ; idx++) {
    idx %= h->size;
    if (skiptombs && h->keys[idx] == tombstone) continue;
    if (h->keys[idx] == 0) return idx;
    if (strcmp(h->keys[idx], key)) continue;
    return idx;
  }
}

struct htable *hcreate(uint32_t size);
char *hget(htable *h, const char *key);
void hset(htable *h, const char *key, const char *value);
void hdel(htable *h, const char *key);
void hdestroy(htable *h);

static void grow(htable *old) {
  htable new = {
    .size = old->size * 2,
  };
  new.keys = calloc(new.size * 2, sizeof *new.keys);
  new.values = new.keys + new.size;

  for (uint32_t i = 0; i < old->size; i++)
    if (old->keys[i] != tombstone && old->keys[i] != 0) {
      uint32_t newi = find(&new, old->keys[i], 0);
      new.keys[newi] = old->keys[i];
      new.values[newi] = old->values[i];
      new.used++;
    }
  free(old->keys);
  *old = new;
}

char *hget(htable *h, const char *key) {
  uint32_t idx = find(h, key, 1);
  return h->keys[idx] ? h->values[idx] : 0;
}

void hset(htable *h, const char *key, const char *value) {
  if (h->used >= h->size/2) grow(h);

  uint32_t idx = find(h, key, 0);
  if (h->keys[idx] != tombstone && h->keys[idx] != 0)
    free(h->values[idx]);
  else
    h->keys[idx] = strdup(key);
  h->values[idx] = strdup(value);
  h->used++;
}

void hdel(htable *h, const char *key) {
  uint32_t idx = find(h, key, 1);
  if (h->keys[idx] != 0) {
    free(h->keys[idx]);
    free(h->values[idx]);
    h->keys[idx % h->size] = tombstone;
  }
}

void hdestroy(htable *h) {
  for (uint32_t i = 0; i < h->size; i++) {
    if (h->keys[i] != tombstone && h->keys[i] != 0) {
      free(h->keys[i]);
      free(h->values[i]);
    }
  }
  free(h->keys);
  free(h);
}

struct htable *hcreate(uint32_t size) {
  struct htable *h = malloc(sizeof *h);
  h->used = 0;
  h->size = size;
  h->keys = calloc(h->size * 2, sizeof *h->keys);
  h->values = h->keys + h->size;
  return h;
}

void hwalk(htable *h, void (*f)(const char *k, const char *v, void *), void *context) {
  for (uint32_t i = 0; i < h->size; i++)
    if (h->keys[i] != tombstone && h->keys[i] != 0)
      f(h->keys[i], h->values[i], context);
}
