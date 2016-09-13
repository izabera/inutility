typedef struct htable htable;
htable *htable_new(size_t size);
char *htable_get(htable *h, const char *key);
void htable_set(htable *h, const char *key, const char *val);
void htable_del(htable *h, const char *key);
void htable_destroy(htable *h);
