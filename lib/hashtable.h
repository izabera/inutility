typedef struct htable htable;
htable *htable_new(size_t size);
char *htable_get(htable *h, const char *key);
void htable_set(htable *h, const char *key, const char *val);
void htable_del(htable *h, const char *key);
void htable_destroy(htable *h);

// macros for putting numbers in hash tables that only accept null terminated strings

#define bytetohex(num) 'A' + ((num)&15), 'A' + (((num)>>4)&255)   // not actually hex but it's fine

#define getbyte(num, pos) \
  ({ union { uint8_t u8[8]; uint64_t u64; } u = { .u64 = (num) }; u.u8[(pos)]; })

#define numtohex(num) \
  _Generic(num, \
      uint8_t: (char []) { bytetohex((num)), 0 },  \
       int8_t: (char []) { bytetohex((num)), 0 },  \
     uint16_t: (char []) { bytetohex(getbyte((num), 0)), bytetohex(getbyte((num), 1)), 0 }, \
      int16_t: (char []) { bytetohex(getbyte((num), 0)), bytetohex(getbyte((num), 1)), 0 }, \
     uint32_t: (char []) { bytetohex(getbyte((num), 0)), bytetohex(getbyte((num), 1)),      \
                           bytetohex(getbyte((num), 2)), bytetohex(getbyte((num), 3)), 0 }, \
      int32_t: (char []) { bytetohex(getbyte((num), 0)), bytetohex(getbyte((num), 1)),      \
                           bytetohex(getbyte((num), 2)), bytetohex(getbyte((num), 3)), 0 }, \
     uint64_t: (char []) { bytetohex(getbyte((num), 0)), bytetohex(getbyte((num), 1)),      \
                           bytetohex(getbyte((num), 2)), bytetohex(getbyte((num), 3)),      \
                           bytetohex(getbyte((num), 4)), bytetohex(getbyte((num), 5)),      \
                           bytetohex(getbyte((num), 6)), bytetohex(getbyte((num), 7)), 0 }, \
      int64_t: (char []) { bytetohex(getbyte((num), 0)), bytetohex(getbyte((num), 1)),      \
                           bytetohex(getbyte((num), 2)), bytetohex(getbyte((num), 3)),      \
                           bytetohex(getbyte((num), 4)), bytetohex(getbyte((num), 5)),      \
                           bytetohex(getbyte((num), 6)), bytetohex(getbyte((num), 7)), 0 }, \
      default: "ERROR" )
