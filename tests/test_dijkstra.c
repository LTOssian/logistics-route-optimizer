#include "dijkstra.h"
#include "graph.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

static int error_count = 0;
static int check_count = 0;
static int total_errors = 0;

/* --- petites fonctions de vérification --- */

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

static void expect_inf(const char *label, double value) {
  check_count++;
  if (!isinf(value)) {
    error_count++;
    printf("  [FAIL] %s: got %f, expected inf\n", label, value);
  } else {
    printf("  [ok]   %s: inf\n", label);
  }
}

static void expect_path(const char *label, const int *path, int path_length,
                        const int *expected_path, int expected_length) {
  check_count++;
  int is_equal = (path != NULL) && (path_length == expected_length);

  if (is_equal) {
    for (int index = 0; index < path_length; index++) {
      if (path[index] != expected_path[index])
        is_equal = 0;
    }
  }

  if (!is_equal) {
    error_count++;
    printf("  [FAIL] %s\n", label);
  } else {
    printf("  [ok]   %s\n", label);
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

/* --- test 1 : un petit graphe où le détour est plus court --- */

/* Les mêmes vérifications pour les deux versions de Dijkstra. */
static void check_basic(const char *version, DijkstraResult *result) {
  printf(" version : %s\n", version);
  expect_int("resultat non NULL", result != NULL, 1);
  if (result == NULL)
    return;

  expect_double("distance vers 0", result->distances[0], 0.0);
  expect_double("distance vers 1", result->distances[1], 1.0);
  expect_double("distance vers 2 (detour)", result->distances[2], 2.0);
  expect_double("distance vers 3", result->distances[3], 3.0);
  expect_int("previous[0] == -1", result->previous[0], -1);
  expect_int("previous[2] == 1", result->previous[2], 1);
  expect_int("settled_count == 4", result->settled_count, 4);

  int path_length = 0;
  int *path = dijkstra_path(result, 3, &path_length);
  int expected_path[] = {0, 1, 2, 3};
  expect_path("chemin 0 -> 3", path, path_length, expected_path, 4);
  free(path);
}

static void test_basic(void) {
  printf("test_basic\n");

  Graph *graph = graph_new(4);
  graph_add_edge(graph, 0, 1, 1.0);
  graph_add_edge(graph, 1, 2, 1.0);
  graph_add_edge(graph, 0, 2, 3.0); // 0->1->2 vaut 2, plus court que 0->2 = 3
  graph_add_edge(graph, 2, 3, 1.0);

  DijkstraResult *naive = dijkstra_naive(graph, 0);
  DijkstraResult *heap = dijkstra(graph, 0);
  check_basic("naive", naive);
  check_basic("tas", heap);

  dijkstra_result_free(naive);
  dijkstra_result_free(heap);
  graph_free(graph);
  print_suite_result("test_basic");
}

/* --- test 2 : un nœud inatteignable --- */

static void check_unreachable(const char *version, DijkstraResult *result) {
  printf(" version : %s\n", version);
  expect_int("resultat non NULL", result != NULL, 1);
  if (result == NULL)
    return;

  expect_double("distance vers 1", result->distances[1], 2.0);
  expect_inf("distance vers 2 (isole)", result->distances[2]);

  int path_length = -1;
  int *path = dijkstra_path(result, 2, &path_length);
  expect_int("pas de chemin vers 2", path == NULL, 1);
  expect_int("path_length == 0", path_length, 0);
}

static void test_unreachable(void) {
  printf("test_unreachable\n");

  Graph *graph = graph_new(3);
  graph_add_edge(graph, 0, 1, 2.0);
  // le nœud 2 n'a aucune arête : il est isolé

  DijkstraResult *naive = dijkstra_naive(graph, 0);
  DijkstraResult *heap = dijkstra(graph, 0);
  check_unreachable("naive", naive);
  check_unreachable("tas", heap);

  dijkstra_result_free(naive);
  dijkstra_result_free(heap);
  graph_free(graph);
  print_suite_result("test_unreachable");
}

/* --- test 3 : le graphe jouet toy1 --- */

static void check_toy1(const char *version, DijkstraResult *result) {
  printf(" version : %s\n", version);
  expect_int("resultat non NULL", result != NULL, 1);
  if (result == NULL)
    return;

  double expected_distances[] = {0.0, 8.0, 9.0, 5.0, 7.0};
  for (int index = 0; index < 5; index++)
    expect_double("distance", result->distances[index],
                  expected_distances[index]);

  int path_length = 0;
  int *path = dijkstra_path(result, 4, &path_length);
  int expected_path[] = {0, 3, 4};
  expect_path("chemin 0 -> 4", path, path_length, expected_path, 3);
  free(path);

  path = dijkstra_path(result, 2, &path_length);
  int expected_path_2[] = {0, 3, 1, 2};
  expect_path("chemin 0 -> 2", path, path_length, expected_path_2, 4);
  free(path);
}

static void test_toy1(void) {
  printf("test_toy1\n");

  Graph *graph = graph_load_csv("data/toy/toy1.csv");
  expect_int("toy1 charge", graph != NULL, 1);
  if (graph == NULL) {
    print_suite_result("test_toy1");
    return;
  }

  DijkstraResult *naive = dijkstra_naive(graph, 0);
  DijkstraResult *heap = dijkstra(graph, 0);
  check_toy1("naive", naive);
  check_toy1("tas", heap);

  dijkstra_result_free(naive);
  dijkstra_result_free(heap);
  graph_free(graph);
  print_suite_result("test_toy1");
}

/* --- test 4 : dijkstra_query (arrêt quand la cible sort du tas) --- */

static void test_query(void) {
  printf("test_query\n");

  Graph *graph = graph_load_csv("data/toy/toy1.csv");
  expect_int("toy1 charge", graph != NULL, 1);
  if (graph == NULL) {
    print_suite_result("test_query");
    return;
  }

  DijkstraResult *full = dijkstra(graph, 0);
  DijkstraResult *query = dijkstra_query(graph, 0, 4);

  if (full != NULL && query != NULL) {
    expect_double("distance 0 -> 4", query->distances[4], 7.0);
    expect_int("query ferme moins ou autant de noeuds",
               query->settled_count <= full->settled_count, 1);
  } else {
    expect_int("resultats non NULL", 0, 1);
  }

  // source == cible : un seul nœud fermé
  DijkstraResult *same = dijkstra_query(graph, 2, 2);
  if (same != NULL) {
    expect_double("source == cible : distance 0", same->distances[2], 0.0);
    expect_int("source == cible : settled == 1", same->settled_count, 1);
  } else {
    expect_int("same non NULL", 0, 1);
  }

  dijkstra_result_free(same);
  dijkstra_result_free(query);
  dijkstra_result_free(full);
  graph_free(graph);
  print_suite_result("test_query");
}

/* --- test 5 : paramètres invalides --- */

static void test_invalid(void) {
  printf("test_invalid\n");

  expect_int("naive(NULL) -> NULL", dijkstra_naive(NULL, 0) == NULL, 1);
  expect_int("dijkstra(NULL) -> NULL", dijkstra(NULL, 0) == NULL, 1);
  expect_int("query(NULL) -> NULL", dijkstra_query(NULL, 0, 0) == NULL, 1);

  Graph *graph = graph_new(2);
  if (graph != NULL) {
    expect_int("naive source hors bornes",
               dijkstra_naive(graph, 2) == NULL, 1);
    expect_int("dijkstra source hors bornes", dijkstra(graph, 2) == NULL, 1);
    expect_int("dijkstra source negative", dijkstra(graph, -1) == NULL, 1);
    expect_int("query cible hors bornes",
               dijkstra_query(graph, 0, 2) == NULL, 1);
    graph_free(graph);
  }

  expect_int("dijkstra_path(NULL) -> NULL",
             dijkstra_path(NULL, 0, NULL) == NULL, 1);
  dijkstra_result_free(NULL);

  print_suite_result("test_invalid");
}

int main(void) {
  test_basic();
  test_unreachable();
  test_toy1();
  test_query();
  test_invalid();

  printf("Toutes les suites sont terminees (%d erreurs).\n", total_errors);
  return (total_errors == 0) ? 0 : 1;
}