#include "graph.h"
#include <stdio.h>

static int g_errors = 0;
static int g_checks = 0;
static int g_total_errors = 0;

static void expect_int(const char *label, int value, int expected) {
  g_checks++;
  if (value != expected) {
    g_errors++;
    printf("  [FAIL] %s: got %d, expected %d\n", label, value, expected);
  } else {
    printf("  [ok]   %s: %d\n", label, value);
  }
}

static void expect_double(const char *label, double value, double expected) {
  g_checks++;
  double diff = value - expected;
  if (diff < 0)
    diff = -diff;
  if (diff > 1e-9) {
    g_errors++;
    printf("  [FAIL] %s: got %f, expected %f\n", label, value, expected);
  } else {
    printf("  [ok]   %s: %f\n", label, value);
  }
}

static void print_suite_result(const char *name) {
  const char *state = (g_errors > 0) ? "FAIL" : "PASS";
  printf("=> %s: TESTS %s (%d checks, %d errors)\n\n", name, state, g_checks,
         g_errors);
  g_total_errors += g_errors;
  g_errors = 0;
  g_checks = 0;
}

/* --- helpers de test --- */

static int out_degree(const Graph *g, int u) {
  int d = 0;
  for (Edge *e = g->adj[u]; e != NULL; e = e->next)
    d++;
  return d;
}

static int has_arc(const Graph *g, int u, int v) {
  for (Edge *e = g->adj[u]; e != NULL; e = e->next) {
    if (e->to == v)
      return 1;
  }
  return 0;
}

static int has_edge(const Graph *g, int u, int v, double w) {
  for (Edge *e = g->adj[u]; e != NULL; e = e->next) {
    double diff = e->weight - w;
    if (diff < 0)
      diff = -diff;
    if (e->to == v && diff <= 1e-9)
      return 1;
  }
  return 0;
}

/* --- suites --- */

static void test_graph_new(void) {
  printf("test_graph_new\n");

  Graph *g = graph_new(5);
  expect_int("graph_new(5) != NULL", g != NULL, 1);

  if (g != NULL) {
    expect_int("n == 5", g->n, 5);
    expect_int("m == 0", g->m, 0);
    expect_int("adj != NULL", g->adj != NULL, 1);

    int empty = 1;
    for (int i = 0; i < g->n; i++) {
      if (g->adj[i] != NULL)
        empty = 0;
    }
    expect_int("toutes les listes sont vides", empty, 1);

    graph_free(g);
  }

  expect_int("graph_new(0) == NULL", graph_new(0) == NULL, 1);
  expect_int("graph_new(-3) == NULL", graph_new(-3) == NULL, 1);

  print_suite_result("test_graph_new");
}

static void test_graph_add_edge(void) {
  printf("test_graph_add_edge\n");

  Graph *g = graph_new(3);
  expect_int("graph_new(3) != NULL", g != NULL, 1);
  if (g == NULL) {
    print_suite_result("test_graph_add_edge");
    return;
  }

  expect_int("add 0->1 (2.5)", graph_add_edge(g, 0, 1, 2.5), 1);
  expect_int("m == 1", g->m, 1);
  expect_int("0->1 existe", has_edge(g, 0, 1, 2.5), 1);
  expect_double("poids de la tete 0->1", g->adj[0]->weight, 2.5);

  expect_int("add 0->2 (3.0)", graph_add_edge(g, 0, 2, 3.0), 1);
  expect_int("m == 2", g->m, 2);
  expect_int("insertion en tete (0->2)", g->adj[0]->to, 2);

  expect_int("u hors bornes -> 0", graph_add_edge(g, 3, 0, 1.0), 0);
  expect_int("v hors bornes -> 0", graph_add_edge(g, 0, 3, 1.0), 0);
  expect_int("poids negatif -> 0", graph_add_edge(g, 0, 1, -1.0), 0);
  expect_int("graphe NULL -> 0", graph_add_edge(NULL, 0, 1, 1.0), 0);
  expect_int("m inchange apres echecs", g->m, 2);

  graph_free(g);
  print_suite_result("test_graph_add_edge");
}

static void test_graph_load_csv(void) {
  printf("test_graph_load_csv\n");

  Graph *g = graph_load_csv("data/toy/toy1.csv");
  expect_int("toy1 charge", g != NULL, 1);

  if (g != NULL) {
    expect_int("n == 5", g->n, 5);
    expect_int("m == 10", g->m, 10);

    expect_int("deg[0] == 2", out_degree(g, 0), 2);
    expect_int("deg[1] == 2", out_degree(g, 1), 2);
    expect_int("deg[2] == 1", out_degree(g, 2), 1);
    expect_int("deg[3] == 3", out_degree(g, 3), 3);
    expect_int("deg[4] == 2", out_degree(g, 4), 2);

    expect_int("0->1 (10.0)", has_edge(g, 0, 1, 10.0), 1);
    expect_int("0->3 (5.0)", has_edge(g, 0, 3, 5.0), 1);
    expect_int("1->2 (1.0)", has_edge(g, 1, 2, 1.0), 1);
    expect_int("3->4 (2.0)", has_edge(g, 3, 4, 2.0), 1);
    expect_int("4->0 (7.0)", has_edge(g, 4, 0, 7.0), 1);

    expect_int("pas d'arete 1->0 (oriente)", has_arc(g, 1, 0), 0);

    graph_free(g);
  }

  expect_int("fichier absent -> NULL",
             graph_load_csv("data/toy/nope.csv") == NULL, 1);
  expect_int("chemin NULL -> NULL", graph_load_csv(NULL) == NULL, 1);
  expect_int("m incoherent -> NULL",
             graph_load_csv("tests/fixtures/bad_m.csv") == NULL, 1);
  expect_int("noeud hors bornes -> NULL",
             graph_load_csv("tests/fixtures/bad_node.csv") == NULL, 1);

  print_suite_result("test_graph_load_csv");
}

static void test_graph_free(void) {
  printf("test_graph_free\n");

  graph_free(NULL);

  Graph *g = graph_new(2);
  if (g != NULL) {
    graph_add_edge(g, 0, 1, 1.0);
    graph_add_edge(g, 1, 0, 2.0);
    graph_free(g);
  }

  expect_int("graph_free(NULL) et graphe non vide OK", 1, 1);

  print_suite_result("test_graph_free");
}

int main(void) {
  test_graph_new();
  test_graph_add_edge();
  test_graph_load_csv();
  test_graph_free();

  printf("Toutes les suites sont terminees (%d erreurs).\n", g_total_errors);
  return (g_total_errors == 0) ? 0 : 1;
}
