#include "rng.h"

#include <stdint.h>
#include <stdio.h>

static int error_count = 0;
static int check_count = 0;
static int total_errors = 0;

static void expect_int(const char *label, int value, int expected) {
  check_count++;
  if (value != expected) {
    error_count++;
    printf("  [FAIL] %s: got %d, expected %d\n", label, value, expected);
  } else {
    printf("  [ok]   %s: %d\n", label, value);
  }
}

static void expect_u64(const char *label, uint64_t value, uint64_t expected) {
  check_count++;
  if (value != expected) {
    error_count++;
    printf("  [FAIL] %s: got %llu, expected %llu\n", label,
           (unsigned long long)value, (unsigned long long)expected);
  } else {
    printf("  [ok]   %s: %llu\n", label, (unsigned long long)value);
  }
}

static void print_suite_result(const char *name) {
  const char *state = (error_count > 0) ? "FAIL" : "PASS";
  printf("=> %s: TESTS %s (%d checks, %d errors)\n\n", name, state, check_count,
         error_count);
  total_errors += error_count;
  error_count = 0;
  check_count = 0;
}

/* --- suites --- */

/* Valeur de référence calculée à la main : xorshift64 depuis l'état 1.
 * x = 1 ; x ^= x<<13 -> 8193 ; x ^= x>>7 -> 8257 ; x ^= x<<17 -> 1082269761 */
static void test_rng_known_value(void) {
  printf("test_rng_known_value\n");

  Rng rng;
  rng_seed(&rng, 1);
  expect_u64("premier tirage (graine 1)", rng_next(&rng), 1082269761ULL);

  print_suite_result("test_rng_known_value");
}

/* Même graine -> même suite ; graines différentes -> suites différentes. */
static void test_rng_reproducible(void) {
  printf("test_rng_reproducible\n");

  Rng a;
  Rng b;
  Rng c;
  rng_seed(&a, 42);
  rng_seed(&b, 42);
  rng_seed(&c, 43);

  int same = 1;
  int differs = 0;
  for (int index = 0; index < 1000; index++) {
    uint64_t va = rng_next(&a);
    uint64_t vb = rng_next(&b);
    uint64_t vc = rng_next(&c);
    if (va != vb)
      same = 0;
    if (va != vc)
      differs = 1;
  }
  expect_int("meme graine -> meme suite (1000 tirages)", same, 1);
  expect_int("graines differentes -> suites differentes", differs, 1);

  print_suite_result("test_rng_reproducible");
}

/* Graine 0 : remplacée par une constante non nulle (sinon l'état resterait 0). */
static void test_rng_zero_seed(void) {
  printf("test_rng_zero_seed\n");

  Rng zero;
  Rng constant;
  rng_seed(&zero, 0);
  rng_seed(&constant, 0x9E3779B97F4A7C15ULL);

  expect_int("etat non nul apres seed(0)", zero.state != 0, 1);
  expect_u64("seed(0) == seed(constante)", rng_next(&zero),
             rng_next(&constant));

  /* l'état ne doit jamais retomber à 0 */
  int never_zero = 1;
  for (int index = 0; index < 10000; index++) {
    if (rng_next(&zero) == 0 || zero.state == 0)
      never_zero = 0;
  }
  expect_int("etat jamais nul (10000 tirages)", never_zero, 1);

  print_suite_result("test_rng_zero_seed");
}

/* rng_int : bornes respectées, cas limites, couverture de toutes les valeurs. */
static void test_rng_int(void) {
  printf("test_rng_int\n");

  Rng rng;
  rng_seed(&rng, 7);

  int in_range = 1;
  int seen[10] = {0};
  for (int index = 0; index < 10000; index++) {
    int value = rng_int(&rng, 10);
    if (value < 0 || value >= 10)
      in_range = 0;
    else
      seen[value]++;
  }
  expect_int("rng_int(10) dans [0, 10)", in_range, 1);

  int all_seen = 1;
  int roughly_uniform = 1;
  for (int value = 0; value < 10; value++) {
    if (seen[value] == 0)
      all_seen = 0;
    /* attendu ~1000 par valeur : marge très large pour éviter les faux échecs */
    if (seen[value] < 700 || seen[value] > 1300)
      roughly_uniform = 0;
  }
  expect_int("toutes les valeurs 0..9 apparaissent", all_seen, 1);
  expect_int("repartition grossierement uniforme", roughly_uniform, 1);

  int bound_one = 1;
  for (int index = 0; index < 100; index++) {
    if (rng_int(&rng, 1) != 0)
      bound_one = 0;
  }
  expect_int("rng_int(1) == 0 toujours", bound_one, 1);

  expect_int("rng_int(0) -> 0", rng_int(&rng, 0), 0);
  expect_int("rng_int(-5) -> 0", rng_int(&rng, -5), 0);

  print_suite_result("test_rng_int");
}

/* rng_double : dans [0, 1), moyenne proche de 0.5. */
static void test_rng_double(void) {
  printf("test_rng_double\n");

  Rng rng;
  rng_seed(&rng, 123);

  int in_range = 1;
  double sum = 0.0;
  const int count = 10000;
  for (int index = 0; index < count; index++) {
    double value = rng_double(&rng);
    if (value < 0.0 || value >= 1.0)
      in_range = 0;
    sum += value;
  }
  double mean = sum / count;

  expect_int("rng_double dans [0, 1)", in_range, 1);
  expect_int("moyenne proche de 0.5 (+/- 0.02)", mean > 0.48 && mean < 0.52, 1);

  print_suite_result("test_rng_double");
}

/* Paramètres NULL : pas de crash. */
static void test_rng_invalid(void) {
  printf("test_rng_invalid\n");

  rng_seed(NULL, 1);
  expect_int("rng_seed(NULL) sans crash", 1, 1);
  expect_int("rng_int(NULL, 10) -> 0", rng_int(NULL, 10), 0);

  print_suite_result("test_rng_invalid");
}

int main(void) {
  test_rng_known_value();
  test_rng_reproducible();
  test_rng_zero_seed();
  test_rng_int();
  test_rng_double();
  test_rng_invalid();

  printf("Toutes les suites sont terminees (%d erreurs).\n", total_errors);
  return (total_errors == 0) ? 0 : 1;
}