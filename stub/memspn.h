#define checkbit(mask, byte) (mask[(unsigned char)byte/8] &  1 << (unsigned char)byte % 8)
#define   setbit(mask, byte) (mask[(unsigned char)byte/8] |= 1 << (unsigned char)byte % 8)

size_t bitmemspn(const char *, size_t, const char *, size_t, void *);
size_t sizememspn(const char *, size_t, const char *, size_t, void *, size_t);
