#include "graph.h"
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

/* --- helpers de test --- */

static int out_degree(const Graph *graph, int node) {
  int degree = 0;
  for (Edge *edge = graph->adjacency[node]; edge != NULL; edge = edge->next)
    degree++;
  return degree;
}

static int has_arc(const Graph *graph, int from, int to) {
  for (Edge *edge = graph->adjacency[from]; edge != NULL; edge = edge->next) {
    if (edge->to == to)
      return 1;
  }
  return 0;
}

static int has_edge(const Graph *graph, int from, int to, double weight) {
  for (Edge *edge = graph->adjacency[from]; edge != NULL; edge = edge->next) {
    double difference = edge->weight - weight;
    if (difference < 0)
      difference = -difference;
    if (edge->to == to && difference <= 1e-9)
      return 1;
  }
  return 0;
}

/* --- suites --- */

static void test_graph_new(void) {
  printf("test_graph_new\n");

  Graph *graph = graph_new(5);
  expect_int("graph_new(5) != NULL", graph != NULL, 1);

  if (graph != NULL) {
    expect_int("node_count == 5", graph->node_count, 5);
    expect_int("edge_count == 0", graph->edge_count, 0);
    expect_int("adjacency != NULL", graph->adjacency != NULL, 1);

    int all_empty = 1;
    for (int index = 0; index < graph->node_count; index++) {
      if (graph->adjacency[index] != NULL)
        all_empty = 0;
    }
    expect_int("toutes les listes sont vides", all_empty, 1);

    graph_free(graph);
  }

  expect_int("graph_new(0) == NULL", graph_new(0) == NULL, 1);
  expect_int("graph_new(-3) == NULL", graph_new(-3) == NULL, 1);

  print_suite_result("test_graph_new");
}

static void test_graph_add_edge(void) {
  printf("test_graph_add_edge\n");

  Graph *graph = graph_new(3);
  expect_int("graph_new(3) != NULL", graph != NULL, 1);
  if (graph == NULL) {
    print_suite_result("test_graph_add_edge");
    return;
  }

  expect_int("add 0->1 (2.5)", graph_add_edge(graph, 0, 1, 2.5), 1);
  expect_int("edge_count == 1", graph->edge_count, 1);
  expect_int("0->1 existe", has_edge(graph, 0, 1, 2.5), 1);
  expect_double("poids de la tete 0->1", graph->adjacency[0]->weight, 2.5);

  expect_int("add 0->2 (3.0)", graph_add_edge(graph, 0, 2, 3.0), 1);
  expect_int("edge_count == 2", graph->edge_count, 2);
  expect_int("insertion en tete (0->2)", graph->adjacency[0]->to, 2);

  expect_int("from hors bornes -> 0", graph_add_edge(graph, 3, 0, 1.0), 0);
  expect_int("to hors bornes -> 0", graph_add_edge(graph, 0, 3, 1.0), 0);
  expect_int("poids negatif -> 0", graph_add_edge(graph, 0, 1, -1.0), 0);
  expect_int("graphe NULL -> 0", graph_add_edge(NULL, 0, 1, 1.0), 0);
  expect_int("edge_count inchange apres echecs", graph->edge_count, 2);

  graph_free(graph);
  print_suite_result("test_graph_add_edge");
}

static void test_graph_load_csv(void) {
  printf("test_graph_load_csv\n");

  Graph *graph = graph_load_csv("data/toy/toy1.csv");
  expect_int("toy1 charge", graph != NULL, 1);

  if (graph != NULL) {
    expect_int("node_count == 5", graph->node_count, 5);
    expect_int("edge_count == 10", graph->edge_count, 10);

    expect_int("deg[0] == 2", out_degree(graph, 0), 2);
    expect_int("deg[1] == 2", out_degree(graph, 1), 2);
    expect_int("deg[2] == 1", out_degree(graph, 2), 1);
    expect_int("deg[3] == 3", out_degree(graph, 3), 3);
    expect_int("deg[4] == 2", out_degree(graph, 4), 2);

    expect_int("0->1 (10.0)", has_edge(graph, 0, 1, 10.0), 1);
    expect_int("0->3 (5.0)", has_edge(graph, 0, 3, 5.0), 1);
    expect_int("1->2 (1.0)", has_edge(graph, 1, 2, 1.0), 1);
    expect_int("3->4 (2.0)", has_edge(graph, 3, 4, 2.0), 1);
    expect_int("4->0 (7.0)", has_edge(graph, 4, 0, 7.0), 1);

    expect_int("pas d'arete 1->0 (oriente)", has_arc(graph, 1, 0), 0);

    graph_free(graph);
  }

  expect_int("fichier absent -> NULL",
             graph_load_csv("data/toy/nope.csv") == NULL, 1);
  expect_int("chemin NULL -> NULL", graph_load_csv(NULL) == NULL, 1);
  expect_int("edge_count incoherent -> NULL",
             graph_load_csv("tests/fixtures/bad_m.csv") == NULL, 1);
  expect_int("noeud hors bornes -> NULL",
             graph_load_csv("tests/fixtures/bad_node.csv") == NULL, 1);

  print_suite_result("test_graph_load_csv");
}

static void test_graph_free(void) {
  printf("test_graph_free\n");

  graph_free(NULL);

  Graph *graph = graph_new(2);
  if (graph != NULL) {
    graph_add_edge(graph, 0, 1, 1.0);
    graph_add_edge(graph, 1, 0, 2.0);
    graph_free(graph);
  }

  expect_int("graph_free(NULL) et graphe non vide OK", 1, 1);

  print_suite_result("test_graph_free");
}

int main(void) {
  test_graph_new();
  test_graph_add_edge();
  test_graph_load_csv();
  test_graph_free();

  printf("Toutes les suites sont terminees (%d erreurs).\n", total_errors);
  return (total_errors == 0) ? 0 : 1;
}
