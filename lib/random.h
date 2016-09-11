typedef struct { uint64_t state;  uint64_t inc; } pcg32_random_t;

extern pcg32_random_t globalrand;


uint32_t pcg32_random_r(pcg32_random_t*);
uint32_t pcg32_random(void);
