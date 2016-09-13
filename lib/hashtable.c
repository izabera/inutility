#include "common.h"

// simple hash table with cuckoo hashing

static inline uint32_t nextpow2(uint32_t num) { return 1 << (32-__builtin_clz(num)); } // num != 0

static uint32_t jenkins(const char *s) {
  uint32_t hash = 0;
  for (; *s; s++) {
    hash += *s;
    hash += hash << 10;
    hash ^= hash >> 6;
  }
  hash += (hash << 3);
  hash ^= (hash >> 11);
  hash += (hash << 15);
  return hash;
}

static uint32_t fnv1a(const char *s) {
  uint32_t hash = 2166136261;
  for (; *s; s++) {
    hash ^= *s;
    hash *= 16777619;
  }
  return hash;
}

typedef struct {
  char *key, *val;
} helem;

struct htable {
  size_t current, capacity;
  helem **elem;
};

htable *htable_new(size_t size) {
  htable *h = malloc(sizeof(struct htable));
  if (!h) return NULL;
  size = nextpow2(size < 63 ? 63 : size);
  h->capacity = size;
  h->current = 0;
  h->elem = calloc(size, sizeof(helem*));
  if (!h->elem) {
    free(h);
    return NULL; 
  }
  return h;
}

char *htable_get(htable *h, const char *key) {
  uint32_t hash = jenkins(key) % h->capacity;
  if (h->elem[hash] && !strcmp(key, h->elem[hash]->key)) return h->elem[hash]->val;
  hash = fnv1a(key) % h->capacity;
  if (h->elem[hash] && !strcmp(key, h->elem[hash]->key)) return h->elem[hash]->val;
  return NULL;
}

// the actual cuckoo thing

typedef char *(*dupfunc)(const char *);
static inline int tryempty(htable *, uint32_t, const char *, const char *, dupfunc);
static inline int trykey(htable *, uint32_t, const char *, const char *, dupfunc);
static void rehash(htable *);
static void freeelems(htable *);

static inline char *nodup(const char *str) { return (char *)str; }

// TODO: abort is fine for now but eventually change it
static inline int tryempty(htable *h, uint32_t hash, const char *key, const char *val, dupfunc dup) {
  if (h->elem[hash]) return 0;
  if (!(h->elem[hash] = malloc(sizeof(helem)))) abort();
  if (!(h->elem[hash]->key = dup(key))) abort();
  if (!(h->elem[hash]->val = dup(val))) abort();
  if (++h->current > h->capacity / 2) rehash(h);
  return 1;
}

static inline int trykey(htable *h, uint32_t hash, const char *key, const char *val, dupfunc dup) {
  if (strcmp(key, h->elem[hash]->key)) return 0;
  free(h->elem[hash]->val);
  if (!(h->elem[hash]->val = dup(val))) abort();
  return 1;
}

void htable_set(htable *h, const char *key, const char *val) {
  dupfunc dup = strdup;
  for (int steps = 0; ; steps++) {
    uint32_t hash = jenkins(key) % h->capacity;
    if (tryempty(h, hash, key, val, dup)) return;
    if (trykey(h, hash, key, val, dup)) return;

    hash = fnv1a(key) % h->capacity;
    if (tryempty(h, hash, key, val, dup)) return;
    if (trykey(h, hash, key, val, dup)) return;

    // both were full
    char *oldkey = h->elem[hash]->key, *oldval = h->elem[hash]->val;
    if (!(h->elem[hash]->key = dup(key))) abort();
    if (!(h->elem[hash]->val = dup(val))) abort();
    dup = nodup; // no need to strdup more than once
    key = oldkey;
    val = oldval;

    if (steps == 100) { // log(capacity) (or something) instead of 100?
      rehash(h);
      steps = 0;
    }
  }
}

static void rehash(htable *h) {
  htable tmptable = {
    .elem = calloc(h->capacity * 2, sizeof(helem*)),
    .capacity = h->capacity * 2,
    .current = 0
  };
  for (size_t i = 0; i < h->capacity; i++)
    if (h->elem[i])
      htable_set(&tmptable, h->elem[i]->key, h->elem[i]->val);
  freeelems(h);
  h->capacity *= 2;
  h->elem = tmptable.elem;
}

void htable_del(htable *h, const char *key) {
  uint32_t hash = jenkins(key) % h->capacity;
  if (h->elem[hash] && !strcmp(key, h->elem[hash]->key)) {
    free(h->elem[hash]->key);
    free(h->elem[hash]->val);
    free(h->elem[hash]);
    h->elem[hash] = NULL;
  }
  hash = fnv1a(key) % h->capacity;
  if (h->elem[hash] && !strcmp(key, h->elem[hash]->key)) {
    free(h->elem[hash]->key);
    free(h->elem[hash]->val);
    free(h->elem[hash]);
    h->elem[hash] = NULL;
  }
}

static inline void freeelems(htable *h) {
  for (size_t i = 0; i < h->capacity; i++) {
    if (h->elem[i]) {
      free(h->elem[i]->key);
      free(h->elem[i]->val);
    }
    free(h->elem[i]);
  }
  free(h->elem);
}

void htable_destroy(htable *h) {
  freeelems(h);
  free(h);
}
