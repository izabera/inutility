#define arrsize(a) sizeof(a)/sizeof(*a)
#define min(a,b) ((a) < (b) ? (a) : (b))
#define max(a,b) ((a) > (b) ? (a) : (b))

struct str {
  unsigned char *str;
  size_t len;
};
