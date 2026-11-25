#include "heap.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

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

static void expect_double(const char *label, double value, double expected) {
  check_count++;
  double difference = value - expected;
  if (difference < 0)
    difference = -difference;
  if (difference > 1e-9) {
    error_count++;
    printf("  [FAIL] %s: got %f, expected %f\n", label, value, expected);
  } else {
    printf("  [ok]   %s: %f\n", label, value);
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

/* xorshift64 local : remplaçable par rng_next() une fois rng.{h,c} écrit. */
static uint64_t next_random(uint64_t *state) {
  uint64_t x = *state;
  x ^= x << 13;
  x ^= x >> 7;
  x ^= x << 17;
  *state = x;
  return x;
}

/* Création, état initial et paramètres invalides (NULL, capacité <= 0). */
static void test_heap_basic(void) {
  printf("test_heap_basic\n");

  MinHeap *heap = heap_new(4);
  expect_int("heap_new(4) != NULL", heap != NULL, 1);

  if (heap != NULL) {
    expect_int("size == 0", heap->size, 0);
    expect_int("capacity == 4", heap->capacity, 4);
    expect_int("items != NULL", heap->items != NULL, 1);
    expect_int("heap_is_empty == 1", heap_is_empty(heap), 1);

    /* pop avec out == NULL : échec, le tas n'est pas modifié */
    heap_push(heap, 1.0, 0);
    expect_int("pop avec out NULL -> 0", heap_pop(heap, NULL), 0);
    expect_int("size inchange apres l'echec", heap->size, 1);

    heap_free(heap);
  }

  HeapItem item = {0.0, 0};
  expect_int("heap_new(0) == NULL", heap_new(0) == NULL, 1);
  expect_int("heap_new(-1) == NULL", heap_new(-1) == NULL, 1);
  expect_int("heap_push(NULL) -> 0", heap_push(NULL, 1.0, 0), 0);
  expect_int("heap_pop(NULL) -> 0", heap_pop(NULL, &item), 0);
  expect_int("heap_is_empty(NULL) -> 1", heap_is_empty(NULL), 1);
  heap_free(NULL);
  expect_int("heap_free(NULL) sans crash", 1, 1);

  print_suite_result("test_heap_basic");
}

/* Petit cas lisible, écrit à la main : utile pour déboguer. */
static void test_heap_small(void) {
  printf("test_heap_small\n");

  MinHeap *heap = heap_new(8);
  expect_int("heap_new(8) != NULL", heap != NULL, 1);
  if (heap == NULL) {
    print_suite_result("test_heap_small");
    return;
  }

  /* clés dans le désordre ; le nœud vaut 10 * clé */
  double keys[] = {5.0, 1.0, 4.0, 2.0, 3.0};
  int nodes[] = {50, 10, 40, 20, 30};

  for (int index = 0; index < 5; index++)
    expect_int("push", heap_push(heap, keys[index], nodes[index]), 1);

  expect_int("size == 5", heap->size, 5);
  expect_int("heap_is_empty == 0", heap_is_empty(heap), 0);

  for (int rank = 1; rank <= 5; rank++) {
    HeapItem item;
    expect_int("pop reussi", heap_pop(heap, &item), 1);
    expect_double("cle dans l'ordre", item.key, (double)rank);
    expect_int("noeud associe", item.node, rank * 10);
  }

  expect_int("tas vide a la fin", heap_is_empty(heap), 1);

  heap_free(heap);
  print_suite_result("test_heap_small");
}

/* Force plusieurs realloc (capacité initiale 2 -> au moins 1000). */
static void test_heap_growth(void) {
  printf("test_heap_growth\n");

  MinHeap *heap = heap_new(2);
  expect_int("heap_new(2) != NULL", heap != NULL, 1);
  if (heap == NULL) {
    print_suite_result("test_heap_growth");
    return;
  }

  /* 1000 clés décroissantes : 1000, 999, ..., 1 (nœud = index) */
  int all_pushed = 1;
  for (int index = 0; index < 1000; index++) {
    if (!heap_push(heap, (double)(1000 - index), index))
      all_pushed = 0;
  }
  expect_int("1000 push reussis", all_pushed, 1);
  expect_int("size == 1000", heap->size, 1000);
  expect_int("capacity >= 1000 (realloc)", heap->capacity >= 1000, 1);

  /* la clé k = 1000 - index sort dans l'ordre 1, 2, ..., 1000 */
  int ordered = 1;
  for (int rank = 1; rank <= 1000; rank++) {
    HeapItem item;
    if (!heap_pop(heap, &item) || item.key != (double)rank ||
        item.node != 1000 - rank)
      ordered = 0;
  }
  expect_int("1000 pop dans l'ordre croissant", ordered, 1);
  expect_int("tas vide a la fin", heap_is_empty(heap), 1);

  heap_free(heap);
  print_suite_result("test_heap_growth");
}

/* Pop sur tas vide : échec sans modifier `out`, y compris après un vidage. */
static void test_heap_empty_pop(void) {
  printf("test_heap_empty_pop\n");

  MinHeap *heap = heap_new(4);
  expect_int("heap_new(4) != NULL", heap != NULL, 1);
  if (heap == NULL) {
    print_suite_result("test_heap_empty_pop");
    return;
  }

  HeapItem item = {42.0, 7};
  expect_int("pop sur tas vide -> 0", heap_pop(heap, &item), 0);
  expect_double("out inchange (key)", item.key, 42.0);
  expect_int("out inchange (node)", item.node, 7);

  /* le tas redevient vide après un push + pop */
  expect_int("push", heap_push(heap, 3.0, 1), 1);
  expect_int("pop", heap_pop(heap, &item), 1);
  expect_int("pop suivant -> 0", heap_pop(heap, &item), 0);
  expect_int("heap_is_empty == 1", heap_is_empty(heap), 1);

  heap_free(heap);
  print_suite_result("test_heap_empty_pop");
}

/* Test le plus puissant : clés pseudo-aléatoires avec doublons. */
static void test_heap_random(void) {
  printf("test_heap_random\n");

  MinHeap *heap = heap_new(4);
  expect_int("heap_new(4) != NULL", heap != NULL, 1);
  if (heap == NULL) {
    print_suite_result("test_heap_random");
    return;
  }

  enum { COUNT = 500 };
  int seen[COUNT] = {0};

  uint64_t state = 42; /* graine fixe : test reproductible */
  int pushed = 0;
  for (int index = 0; index < COUNT; index++) {
    /* clés dans [0, 100) : beaucoup de doublons */
    double key = (double)(next_random(&state) % 100);
    if (heap_push(heap, key, index))
      pushed++;
  }
  expect_int("500 push reussis", pushed, COUNT);

  int popped = 0;
  int sorted = 1;
  int nodes_valid = 1;
  double previous = -1.0;
  HeapItem item;
  while (heap_pop(heap, &item)) {
    if (item.key < previous)
      sorted = 0;
    previous = item.key;
    if (item.node < 0 || item.node >= COUNT)
      nodes_valid = 0;
    else
      seen[item.node]++;
    popped++;
  }
  expect_int("500 pop", popped, COUNT);
  expect_int("sortie triee par cle croissante", sorted, 1);
  expect_int("noeuds valides", nodes_valid, 1);

  /* aucune entrée perdue ni dupliquée */
  int each_once = 1;
  for (int index = 0; index < COUNT; index++) {
    if (seen[index] != 1)
      each_once = 0;
  }
  expect_int("chaque noeud sort exactement une fois", each_once, 1);
  expect_int("tas vide a la fin", heap_is_empty(heap), 1);

  heap_free(heap);
  print_suite_result("test_heap_random");
}

int main(void) {
  test_heap_basic();
  test_heap_small();
  test_heap_growth();
  test_heap_empty_pop();
  test_heap_random();

  printf("Toutes les suites sont terminees (%d erreurs).\n", total_errors);
  return (total_errors == 0) ? 0 : 1;
}