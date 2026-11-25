#include "dijkstra.h"
#include "graph.h"
#include "rng.h"

#include <math.h>
#include <stdio.h>

static int error_count = 0;
static int check_count = 0;

static void expect_int(const char *label, int value, int expected) {
  check_count++;
  if (value != expected) {
    error_count++;
    printf("  [FAIL] %s: got %d, expected %d\n", label, value, expected);
  } else {
    printf("  [ok]   %s: %d\n", label, value);
  }
}

/* Oracle : on essaie tous les chemins simples depuis `node`
 * et on garde la plus petite longueur trouvée pour chaque nœud. */
static void explore(const Graph *graph, int node, double length, int *used,
                    double *best) {
  if (length < best[node])
    best[node] = length;

  used[node] = 1;
  for (Edge *edge = graph->adjacency[node]; edge != NULL; edge = edge->next) {
    if (!used[edge->to])
      explore(graph, edge->to, length + edge->weight, used, best);
  }
  used[node] = 0;
}

/* Deux distances sont égales (infini compris). */
static int same(double a, double b) {
  if (isinf(a) || isinf(b))
    return isinf(a) && isinf(b);
  return fabs(a - b) < 1e-9;
}

/* Compare naive, tas et query à l'oracle. Retourne le nombre d'erreurs. */
static int check_graph(const Graph *graph) {
  int errors = 0;
  int n = graph->node_count;
  double best[8];
  int used[8];

  for (int source = 0; source < n; source++) {
    for (int i = 0; i < n; i++) {
      best[i] = INFINITY;
      used[i] = 0;
    }
    explore(graph, source, 0.0, used, best);

    DijkstraResult *naive = dijkstra_naive(graph, source);
    DijkstraResult *heap = dijkstra(graph, source);
    if (naive == NULL || heap == NULL) {
      errors++;
      dijkstra_result_free(naive);
      dijkstra_result_free(heap);
      continue;
    }

    for (int target = 0; target < n; target++) {
      if (!same(naive->distances[target], best[target]))
        errors++;
      if (!same(heap->distances[target], best[target]))
        errors++;

      DijkstraResult *query = dijkstra_query(graph, source, target);
      if (query == NULL || !same(query->distances[target], best[target]))
        errors++;
      dijkstra_result_free(query);
    }

    dijkstra_result_free(naive);
    dijkstra_result_free(heap);
  }
  return errors;
}

static void test_toy1(void) {
  printf("test_toy1\n");
  Graph *graph = graph_load_csv("data/toy/toy1.csv");
  expect_int("toy1 charge", graph != NULL, 1);
  if (graph != NULL) {
    expect_int("toy1 : 0 erreur", check_graph(graph), 0);
    graph_free(graph);
  }
}

/* Arêtes parallèles, poids nul, nœud isolé. */
static void test_special(void) {
  printf("test_special\n");
  Graph *graph = graph_new(4);
  expect_int("graph_new(4)", graph != NULL, 1);
  if (graph != NULL) {
    graph_add_edge(graph, 0, 1, 5.0);
    graph_add_edge(graph, 0, 1, 2.0); /* parallèle, plus courte */
    graph_add_edge(graph, 1, 2, 0.0); /* poids nul */
    /* le nœud 3 est isolé */
    expect_int("cas speciaux : 0 erreur", check_graph(graph), 0);
    graph_free(graph);
  }
}

/* 100 petits graphes aléatoires (graine fixe : résultat reproductible). */
static void test_random(void) {
  printf("test_random\n");
  Rng rng;
  rng_seed(&rng, 2024);

  int errors = 0;
  for (int i = 0; i < 100; i++) {
    int n = 2 + rng_int(&rng, 6); /* 2 à 7 nœuds */
    Graph *graph = graph_new(n);
    if (graph == NULL) {
      errors++;
      continue;
    }
    int edges = rng_int(&rng, 3 * n);
    for (int e = 0; e < edges; e++)
      graph_add_edge(graph, rng_int(&rng, n), rng_int(&rng, n),
                     (double)rng_int(&rng, 10));
    errors += check_graph(graph);
    graph_free(graph);
  }
  expect_int("100 graphes aleatoires : 0 erreur", errors, 0);
}

int main(void) {
  test_toy1();
  test_special();
  test_random();

  printf("=> %d checks, %d erreurs\n", check_count, error_count);
  return (error_count == 0) ? 0 : 1;
}