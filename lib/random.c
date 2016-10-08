#include "common.h"

// *Really* minimal PCG32 code / (c) 2014 M.E. O'Neill / pcg-random.org
// Licensed under Apache License 2.0 (NO WARRANTY, etc. see website)

uint32_t pcg32_random_r(pcg32_random_t* rng)
{
    uint64_t oldstate = rng->state;
    // Advance internal state
    rng->state = oldstate * 6364136223846793005ULL + (rng->inc|1);
    // Calculate output function (XSH RR), uses old state for max ILP
    uint32_t xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
    uint32_t rot = oldstate >> 59u;
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

pcg32_random_t globalrand;

// put this here instead of main in inutility.c
// to allow compilation of programs without inutility.c
__attribute__((constructor)) static void srandtimeNULL() {
  struct timespec t;
  clock_gettime(CLOCK_REALTIME, &t);
  globalrand.state = t.tv_nsec;
  globalrand.inc = t.tv_nsec | 1;
  pcg32_random(); // the first number is always low
}

uint32_t pcg32_random() { return pcg32_random_r(&globalrand); }
