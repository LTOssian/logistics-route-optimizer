#include "graph.h"
#include "points.h"

#include <stdio.h>
#include <stdlib.h>

static int check_count = 0;
static int error_count = 0;

static void expect_true(const char *label, int condition) {
  check_count++;
  if (!condition) {
    error_count++;
    printf("  [FAIL] %s\n", label);
  } else {
    printf("  [ok]   %s\n", label);
  }
}

/* Centre utilisé pour extraire les graphes */
#define CENTER_LAT 45.7578
#define CENTER_LON 4.832

static Graph *load_lyon_500(void) {
  Graph *graph = graph_load_csv("data/osm/lyon_500/edges.csv");
  if (graph == NULL)
    return NULL;
  if (!graph_load_nodes_csv(graph, "data/osm/lyon_500/nodes.csv")) {
    graph_free(graph);
    return NULL;
  }
  return graph;
}

/* Vérifie que le dépôt est bien un nœud valide proche du centre, qu'un nœud
 * est le plus proche de lui-même, et qu'un graphe sans coordonnées est
 * refusé proprement. */
static void test_nearest(void) {
  printf("test_nearest\n");
  Graph *graph = load_lyon_500();
  expect_true("lyon_500 + coordonnees charges", graph != NULL);
  if (graph == NULL)
    return;

  int depot = nearest_node(graph, CENTER_LAT, CENTER_LON);
  printf("         (depot de lyon_500 = noeud %d)\n", depot);
  expect_true("depot valide", depot >= 0 && depot < graph->node_count);

  expect_true("le plus proche du noeud 37 est le noeud 37",
              nearest_node(graph, graph->lat[37], graph->lon[37]) == 37);
  expect_true("graphe NULL -> -1", nearest_node(NULL, 0.0, 0.0) == -1);
  graph_free(graph);

  Graph *no_coords = graph_load_csv("data/osm/lyon_500/edges.csv");
  if (no_coords != NULL)
    expect_true("graphe sans coordonnees -> -1",
                nearest_node(no_coords, CENTER_LAT, CENTER_LON) == -1);
  graph_free(no_coords);
}

/* Vérifie le tirage des clients : bonne taille, dépôt en case 0, aucun
 * doublon, dépôt jamais tiré comme client, et même graine = mêmes points
 * (reproductibilité exigée par le plan). */
static void test_pick(void) {
  printf("test_pick\n");
  int node_count = 154;
  int depot = 7;
  int n = 20;

  int *points = pick_points(node_count, depot, n, 42);
  expect_true("pick_points non NULL", points != NULL);
  if (points == NULL)
    return;

  expect_true("points[0] == depot", points[0] == depot);

  int valid = 1;
  int no_duplicate = 1;
  int *seen = calloc((size_t)node_count, sizeof(int));
  for (int i = 0; i <= n; i++) {
    if (points[i] < 0 || points[i] >= node_count) {
      valid = 0;
      continue;
    }
    if (seen[points[i]])
      no_duplicate = 0;
    seen[points[i]] = 1;
  }
  free(seen);
  expect_true("tous les ids sont dans [0, node_count)", valid);
  expect_true("aucun doublon (le depot n'est pas tire comme client)",
              no_duplicate);

  int *again = pick_points(node_count, depot, n, 42);
  int same = (again != NULL);
  for (int i = 0; same && i <= n; i++)
    if (again[i] != points[i])
      same = 0;
  expect_true("meme graine -> memes points", same);

  int *other = pick_points(node_count, depot, n, 43);
  int different = 0;
  for (int i = 1; other != NULL && i <= n; i++)
    if (other[i] != points[i])
      different = 1;
  expect_true("graine differente -> autres points", different);

  free(other);
  free(again);
  free(points);

  /* Cas limite : on prend TOUS les autres nœuds. La boucle doit finir. */
  int *all = pick_points(5, 0, 4, 42);
  expect_true("N = node_count - 1 : fonctionne", all != NULL);
  free(all);

  expect_true("N trop grand -> NULL", pick_points(5, 0, 5, 42) == NULL);
  expect_true("N = 0 -> NULL", pick_points(5, 0, 0, 42) == NULL);
  expect_true("depot hors bornes -> NULL", pick_points(5, 9, 2, 42) == NULL);
}

/* lance les tests et renvoie 1 en cas d'erreur */
int main(void) {
  test_nearest();
  test_pick();

  printf("=> %d checks, %d erreurs\n", check_count, error_count);
  return error_count == 0 ? 0 : 1;
}