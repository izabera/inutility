#ifndef NO_ALLOC_WRAPPERS

static inline void* xmalloc(size_t size) {
  void *x = malloc(size);
  if (unlikely(!x)) exit(errno);
  return x;
}

static inline void* xaligned_alloc(size_t alignment, size_t size) {
  void *x = aligned_alloc(alignment, size);
  if (unlikely(!x)) exit(errno);
  return x;
}

static inline void* xrealloc(void *ptr, size_t size) {
  void *x = realloc(ptr, size);
  if (unlikely(!x)) exit(errno);
  return x;
}

static inline void* xcalloc(size_t nmemb, size_t size) {
  void *x = calloc(nmemb, size);
  if (unlikely(!x)) exit(errno);
  return x;
}

#define malloc(x)          xmalloc(x)
#define aligned_alloc(x,y) xaligned_alloc(x,y)
#define realloc(x,y)       xrealloc(x,y)
#define calloc(x,y)        xcalloc(x,y)

#endif
