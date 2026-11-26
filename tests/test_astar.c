#include "astar.h"
#include "dijkstra.h"
#include "graph.h"
#include "rng.h"

#include <math.h>
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

static void expect_double(const char *label, double value, double expected,
                          double tolerance) {
  check_count++;
  if (fabs(value - expected) > tolerance) {
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

/* Tolérance de la consigne : fabs(a - b) <= 1e-9 * fmax(1, fabs(a)). */
static int same_distance(double a, double b) {
  if (isinf(a) || isinf(b))
    return isinf(a) && isinf(b);
  return fabs(a - b) <= 1e-9 * fmax(1.0, fabs(a));
}

/* --- suite 1 : haversine --- */

static void test_haversine(void) {
  printf("test_haversine\n");

  expect_double("meme point -> 0", haversine_m(45.0, 4.0, 45.0, 4.0), 0.0, 1e-9);

  double one_degree = EARTH_RADIUS_M * 3.14159265358979323846 / 180.0;
  expect_double("1 degre de latitude", haversine_m(0.0, 0.0, 1.0, 0.0),
                one_degree, 1e-6);

  double ab = haversine_m(48.8566, 2.3522, 45.7640, 4.8357);
  double ba = haversine_m(45.7640, 4.8357, 48.8566, 2.3522);
  expect_double("symetrie", ab, ba, 1e-6);
  expect_int("Paris-Lyon entre 390 et 394 km", ab > 390000.0 && ab < 394000.0,
             1);

  print_suite_result("test_haversine");
}

/* --- suite 2 : graphe jouet avec coordonnees --- */

static void test_toy1(void) {
  printf("test_toy1\n");

  Graph *graph = graph_load_csv("data/toy/toy1.csv");
  expect_int("toy1 charge", graph != NULL, 1);
  if (graph == NULL) {
    print_suite_result("test_toy1");
    return;
  }

  /* sans coordonnees : refus */
  expect_int("sans coordonnees -> NULL", astar(graph, 0, 4) == NULL, 1);

  expect_int("nodes charges",
             graph_load_nodes_csv(graph, "data/toy/toy1_nodes.csv"), 1);

  DijkstraResult *result = astar(graph, 0, 4);
  expect_int("astar(0, 4) non NULL", result != NULL, 1);
  if (result != NULL) {
    expect_double("distance 0 -> 4", result->distances[4], 7.0, 1e-9);

    int length = 0;
    int *path = dijkstra_path(result, 4, &length);
    int expected_path[] = {0, 3, 4};
    int path_ok = (path != NULL && length == 3);
    for (int index = 0; path_ok && index < 3; index++)
      path_ok = (path[index] == expected_path[index]);
    expect_int("chemin 0 -> 4 = 0-3-4", path_ok, 1);
    free(path);
    dijkstra_result_free(result);
  }

  /* toutes les cibles depuis 0 : memes distances que Dijkstra */
  int all_equal = 1;
  for (int target = 0; target < graph->node_count; target++) {
    DijkstraResult *a = astar(graph, 0, target);
    DijkstraResult *d = dijkstra_query(graph, 0, target);
    if (a == NULL || d == NULL ||
        !same_distance(a->distances[target], d->distances[target]))
      all_equal = 0;
    dijkstra_result_free(a);
    dijkstra_result_free(d);
  }
  expect_int("jouet : A* == Dijkstra pour toutes les cibles", all_equal, 1);

  DijkstraResult *same = astar(graph, 2, 2);
  if (same != NULL) {
    expect_double("source == cible : distance 0", same->distances[2], 0.0, 1e-9);
    expect_int("source == cible : settled == 1", same->settled_count, 1);
  } else {
    expect_int("same non NULL", 0, 1);
  }
  dijkstra_result_free(same);

  expect_int("source hors bornes -> NULL", astar(graph, 5, 0) == NULL, 1);
  expect_int("cible negative -> NULL", astar(graph, 0, -1) == NULL, 1);
  expect_int("graphe NULL -> NULL", astar(NULL, 0, 0) == NULL, 1);

  graph_free(graph);
  print_suite_result("test_toy1");
}

/* --- suite 3 : graphe geometrique aleatoire (graine 42) --- */

#define NEIGHBORS 4

static Graph *make_geometric_graph(Rng *rng, int n) {
  Graph *graph = graph_new(n);
  if (graph == NULL)
    return NULL;

  /* points dans un carre d'environ 11 km x 8 km autour de Lyon */
  for (int node = 0; node < n; node++) {
    double lat = 45.70 + 0.1 * rng_double(rng);
    double lon = 4.80 + 0.1 * rng_double(rng);
    if (!graph_set_coords(graph, node, lat, lon)) {
      graph_free(graph);
      return NULL;
    }
  }

  /* chaque point est relie (dans les deux sens) a ses plus proches voisins */
  for (int node = 0; node < n; node++) {
    int picked[NEIGHBORS];
    for (int k = 0; k < NEIGHBORS; k++) {
      int best = -1;
      double best_distance = 0.0;
      for (int other = 0; other < n; other++) {
        if (other == node)
          continue;
        int already = 0;
        for (int p = 0; p < k; p++)
          if (picked[p] == other)
            already = 1;
        if (already)
          continue;
        double d = haversine_m(graph->lat[node], graph->lon[node],
                               graph->lat[other], graph->lon[other]);
        if (best < 0 || d < best_distance) {
          best = other;
          best_distance = d;
        }
      }
      picked[k] = best;
      /* poids >= haversine : heuristique admissible */
      double weight = best_distance * (1.0 + 0.5 * rng_double(rng));
      if (!graph_add_edge(graph, node, best, weight) ||
          !graph_add_edge(graph, best, node, weight)) {
        graph_free(graph);
        return NULL;
      }
    }
  }
  return graph;
}

static void test_random_pairs(void) {
  printf("test_random_pairs\n");

  Rng rng;
  rng_seed(&rng, 42);

  const int n = 400;
  const int pairs = 500;
  Graph *graph = make_geometric_graph(&rng, n);
  expect_int("graphe geometrique construit", graph != NULL, 1);
  if (graph == NULL) {
    print_suite_result("test_random_pairs");
    return;
  }

  int mismatches = 0;
  int reachable = 0;
  long settled_astar = 0;
  long settled_dijkstra = 0;

  for (int index = 0; index < pairs; index++) {
    int source = rng_int(&rng, n);
    int target = rng_int(&rng, n);

    DijkstraResult *a = astar(graph, source, target);
    DijkstraResult *d = dijkstra_query(graph, source, target);
    if (a == NULL || d == NULL) {
      mismatches++;
    } else {
      if (!same_distance(a->distances[target], d->distances[target]))
        mismatches++;
      if (!isinf(d->distances[target]))
        reachable++;
      settled_astar += a->settled_count;
      settled_dijkstra += d->settled_count;
    }
    dijkstra_result_free(a);
    dijkstra_result_free(d);
  }

  printf("  settled total : astar = %ld, dijkstra_query = %ld (%.1f %%)\n",
         settled_astar, settled_dijkstra,
         100.0 * (double)settled_astar / (double)settled_dijkstra);

  expect_int("500 paires : 0 difference de distance", mismatches, 0);
  expect_int("au moins 100 paires atteignables", reachable >= 100, 1);
  expect_int("A* ferme moins de noeuds au total que dijkstra_query",
             settled_astar < settled_dijkstra, 1);

  graph_free(graph);
  print_suite_result("test_random_pairs");
}

int main(void) {
  test_haversine();
  test_toy1();
  test_random_pairs();

  printf("Toutes les suites sont terminees (%d erreurs).\n", total_errors);
  return (total_errors == 0) ? 0 : 1;
}