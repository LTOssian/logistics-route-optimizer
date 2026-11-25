#include "rng.h"

#include <stddef.h>

void rng_seed(Rng *rng, uint64_t seed) {
  if (rng == NULL)
    return;
  rng->state = (seed != 0) ? seed : 0x9E3779B97F4A7C15ULL;
}

uint64_t rng_next(Rng *rng) {
  uint64_t x = rng->state;
  x ^= x << 13;
  x ^= x >> 7;
  x ^= x << 17;
  rng->state = x;
  return x;
}

int rng_int(Rng *rng, int bound) {
  if (rng == NULL || bound <= 0)
    return 0;
  return (int)(rng_next(rng) % (uint64_t)bound);
}

double rng_double(Rng *rng) {
  return (double)(rng_next(rng) >> 11) / 9007199254740992.0; /* 2^53 */
}