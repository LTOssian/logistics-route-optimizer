#ifndef RNG_H
#define RNG_H

#include <stdint.h>

/**
 * @file rng.h
 * @brief Générateur pseudo-aléatoire xorshift64, reproductible.
 */

/** @brief État du générateur. */
typedef struct Rng {
  uint64_t state; /**< état interne (jamais nul). */
} Rng;

/**
 * @brief Initialise le générateur avec une graine.
 * @param seed graine (0 est remplacé par une constante non nulle).
 */
void rng_seed(Rng *rng, uint64_t seed);

/** @brief Tire un entier 64 bits. */
uint64_t rng_next(Rng *rng);

/**
 * @brief Tire un entier dans [0, bound).
 * @param bound borne exclusive (bound >= 1), sinon renvoie 0.
 */
int rng_int(Rng *rng, int bound);

/** @brief Tire un réel dans [0, 1). */
double rng_double(Rng *rng);

#endif