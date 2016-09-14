#include "common.h"

// simple hash table with cuckoo hashing

static inline uint32_t nextpow2(uint32_t num) { return 1 << (32-__builtin_clz(num)); } // num != 0

// https://oeis.org/A014234
static const uint64_t primes[] = {
  2, 3, 7, 13, 31, 61, 127, 251, 509, 1021, 2039, 4093, 8191, 16381, 32749,
  65521, 131071, 262139, 524287, 1048573, 2097143, 4194301, 8388593, 16777213,
  33554393, 67108859, 134217689, 268435399, 536870909, 1073741789, 2147483647,
  4294967291
};

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
  size = ilog2(size < 63 ? 63 : size);
  h->capacity = size;
  h->current = 0;
  h->elem = calloc(primes[size], sizeof(helem*));
  if (!h->elem) {
    free(h);
    return NULL; 
  }
  return h;
}

#define cuckoo_threshold 5
char *htable_get(htable *h, const char *key) {
  uint32_t hash = jenkins(key) % primes[h->capacity];
  for (int i = 0; i < cuckoo_threshold; i++) {
    hash = (hash + i) % primes[h->capacity];
    if (h->elem[hash] && !strcmp(key, h->elem[hash]->key)) return h->elem[hash]->val;
  }

  hash = fnv1a(key) % primes[h->capacity];
  for (int i = 0; i < cuckoo_threshold; i++) {
    hash = (hash + i) % primes[h->capacity];
    if (h->elem[hash] && !strcmp(key, h->elem[hash]->key)) return h->elem[hash]->val;
  }
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
  if (++h->current > primes[h->capacity] / 2) rehash(h);
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
  for (size_t steps = 0; ; steps++) {
    uint32_t hash = jenkins(key) % primes[h->capacity];
    for (int i = 0; i < cuckoo_threshold; i++) {
      hash = (hash + i) % primes[h->capacity];
      if (tryempty(h, hash, key, val, dup)) return;
      if (trykey(h, hash, key, val, dup)) return;
    }

    hash = fnv1a(key) % primes[h->capacity];
    for (int i = 0; i < cuckoo_threshold; i++) {
      hash = (hash + i) % primes[h->capacity];
      if (tryempty(h, hash, key, val, dup)) return;
      if (trykey(h, hash, key, val, dup)) return;
    }

    // both were full
    char *oldkey = h->elem[hash]->key, *oldval = h->elem[hash]->val;
    if (!(h->elem[hash]->key = dup(key))) abort();
    if (!(h->elem[hash]->val = dup(val))) abort();
    dup = nodup; // no need to strdup more than once
    key = oldkey;
    val = oldval;

    if (steps == h->capacity) {
      rehash(h);
      steps = 0;
    }
  }
}

static void rehash(htable *h) {
  htable tmptable = {
    .current = 0,
    .capacity = h->capacity + 1,
    .elem = calloc(primes[h->capacity+1], sizeof(helem*)),
  };
  for (size_t i = 0; i < primes[h->capacity]; i++)
    if (h->elem[i])
      htable_set(&tmptable, h->elem[i]->key, h->elem[i]->val);
  freeelems(h);
  h->capacity += 1;
  h->elem = tmptable.elem;
}

void htable_del(htable *h, const char *key) {
  uint32_t hash = jenkins(key) % h->capacity;
  if (h->elem[hash] && !strcmp(key, h->elem[hash]->key)) {
    free(h->elem[hash]->key);
    free(h->elem[hash]->val);
    free(h->elem[hash]);
    h->elem[hash] = NULL;
    h->current--;
    return;
  }
  hash = fnv1a(key) % h->capacity;
  if (h->elem[hash] && !strcmp(key, h->elem[hash]->key)) {
    free(h->elem[hash]->key);
    free(h->elem[hash]->val);
    free(h->elem[hash]);
    h->elem[hash] = NULL;
    h->current--;
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


