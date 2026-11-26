#include "dijkstra.h"
#include "distance_matrix.h"
#include "graph.h"

#include <math.h>
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


static int nearly_equal(double a, double b) {
  double scale = fmax(fabs(a), fabs(b));
  if (scale < 1.0)
    scale = 1.0;
  return fabs(a - b) <= 1e-9 * scale;
}

static void write_file(const char *path, const char *content) {
  FILE *file = fopen(path, "w");
  if (file != NULL) {
    fputs(content, file);
    fclose(file);
  }
}

static const int POINTS[] = {10, 3, 77, 120, 5, 42, 99, 150};
static const int POINT_COUNT = (int)(sizeof POINTS / sizeof POINTS[0]);

/* --- tests --- */

/* Vérifie que la matrice a la bonne taille, que le dépôt est bien à l'indice 0
 * et que la liste des points a été recopiée dans le bon ordre. */
static void test_shape(const DistanceMatrix *m) {
  printf("test_shape\n");
  expect_true("matrice non NULL", m != NULL);
  expect_true("size == nombre de points", m->size == POINT_COUNT);
  expect_true("nodes[0] == depot", m->nodes[0] == POINTS[0]);

  int same_nodes = 1;
  for (int i = 0; i < m->size; i++)
    if (m->nodes[i] != POINTS[i])
      same_nodes = 0;
  expect_true("nodes recopies dans l'ordre", same_nodes);
}

/* Vérifie qu'une case hors de la matrice renvoie NAN au lieu de lire
 * n'importe où en mémoire. */
static void test_get_bounds(const DistanceMatrix *m) {
  printf("test_get_bounds\n");
  expect_true("get hors bornes -> NAN",
              isnan(distance_matrix_get(m, m->size, 0)) &&
                  isnan(distance_matrix_get(m, 0, -1)));
}

/* Vérifie que la distance d'un point à lui-même vaut 0 (la diagonale). */
static void test_diagonal(const DistanceMatrix *m) {
  printf("test_diagonal\n");
  int ok = 1;
  for (int i = 0; i < m->size; i++)
    if (distance_matrix_get(m, i, i) != 0.0)
      ok = 0;
  expect_true("d[i][i] == 0 pour tout i", ok);
}

/* Vérifie que aller de A à B coûte autant que de B à A : le graphe de Lyon
 * est non orienté, donc la matrice doit être symétrique. */
static void test_symmetry(const DistanceMatrix *m) {
  printf("test_symmetry\n");
  int ok = 1;
  for (int i = 0; i < m->size; i++)
    for (int j = 0; j < m->size; j++)
      if (!nearly_equal(distance_matrix_get(m, i, j),
                        distance_matrix_get(m, j, i)))
        ok = 0;
  expect_true("d[i][j] == d[j][i] (tolerance relative 1e-9)", ok);
}

/* Vérifie l'inégalité triangulaire : faire un détour par un point j n'est
 * jamais plus court que d'aller directement de i à k. */
static void test_triangle(const DistanceMatrix *m) {
  printf("test_triangle\n");
  int ok = 1;
  for (int i = 0; i < m->size; i++)
    for (int j = 0; j < m->size; j++)
      for (int k = 0; k < m->size; k++) {
        double direct = distance_matrix_get(m, i, k);
        double detour = distance_matrix_get(m, i, j) +
                        distance_matrix_get(m, j, k);
        if (direct > detour + 1e-9 * fmax(detour, 1.0))
          ok = 0;
      }
  expect_true("d[i][k] <= d[i][j] + d[j][k] pour tout i, j, k", ok);
}

/* Vérifie que chaque case de la matrice est identique au résultat d'un
 * Dijkstra classique (sans arrêt anticipé) lancé à part. */
static void test_matches_dijkstra(const Graph *graph,
                                  const DistanceMatrix *m) {
  printf("test_matches_dijkstra\n");
  int ok = 1;
  for (int i = 0; i < m->size; i++) {
    DijkstraResult *full = dijkstra(graph, m->nodes[i]);
    if (full == NULL) {
      ok = 0;
      continue;
    }
    for (int j = 0; j < m->size; j++)
      if (!nearly_equal(distance_matrix_get(m, i, j),
                        full->distances[m->nodes[j]]))
        ok = 0;
    dijkstra_result_free(full);
  }
  expect_true("chaque case == Dijkstra complet isole", ok);
}

/* Vérifie sur le grand graphe (lyon_4000) que dijkstra_multi s'arrête bien
 * plus tôt qu'un Dijkstra complet (moins de nœuds fermés) tout en donnant
 * exactement les mêmes distances vers les cibles, même si une cible est
 * donnée deux fois. */
static void test_early_stop(void) {
  printf("test_early_stop\n");
  Graph *graph = graph_load_csv("data/osm/lyon_4000/edges.csv");
  expect_true("lyon_4000 charge", graph != NULL);
  if (graph == NULL)
    return;

  /* Si la seule cible est la source elle-même, Dijkstra doit s'arrêter
   * immédiatement : un seul nœud fermé. */
  int source = 0;
  DijkstraResult *full = dijkstra(graph, source);
  DijkstraResult *first = dijkstra_multi(graph, source, &source, 1);
  expect_true("resultats non NULL", full != NULL && first != NULL);

  if (full != NULL && first != NULL) {
    expect_true("source seule : settled == 1", first->settled_count == 1);
    expect_true("multi ferme moins de noeuds que le complet",
                first->settled_count < full->settled_count);
  }

  /* Cibles quelconques : distances identiques au Dijkstra complet. */
  int targets[] = {100, 2000, 37, 100}; /* 100 en double, exprès */
  DijkstraResult *multi = dijkstra_multi(graph, source, targets, 4);
  int ok = (multi != NULL && full != NULL);
  for (int t = 0; ok && t < 4; t++)
    if (multi->distances[targets[t]] != full->distances[targets[t]])
      ok = 0;
  expect_true("multi : distances aux cibles == Dijkstra complet", ok);
  if (multi != NULL && full != NULL)
    expect_true("multi ferme <= noeuds que le complet",
                multi->settled_count <= full->settled_count);

  dijkstra_result_free(multi);
  dijkstra_result_free(first);
  dijkstra_result_free(full);
  graph_free(graph);
}

/* Vérifie l'export puis le réimport CSV : on doit retrouver EXACTEMENT la
 * même matrice. Les fichiers mal formés doivent être refusés. */
static void test_csv(const DistanceMatrix *m) {
  printf("test_csv\n");
  const char *path = "build/test_matrix.csv";

  expect_true("save_csv reussi", distance_matrix_save_csv(m, path));
  DistanceMatrix *loaded = distance_matrix_load_csv(path);
  expect_true("load_csv non NULL", loaded != NULL);

  if (loaded != NULL) {
    expect_true("meme taille", loaded->size == m->size);
    int same = (loaded->size == m->size);
    for (int i = 0; same && i < m->size; i++) {
      if (loaded->nodes[i] != m->nodes[i])
        same = 0;
      for (int j = 0; j < m->size; j++)
        if (distance_matrix_get(loaded, i, j) != distance_matrix_get(m, i, j))
          same = 0;
    }
    expect_true("memes noeuds et memes distances (egalite exacte)", same);
    distance_matrix_free(loaded);
  }

  /* Un point inatteignable (INFINITY) doit survivre à l'aller-retour. */
  write_file(path, "size,2\nnodes,4,9\n0,inf\ninf,0\n");
  DistanceMatrix *with_inf = distance_matrix_load_csv(path);
  expect_true("inf relu comme INFINITY",
              with_inf != NULL && isinf(distance_matrix_get(with_inf, 0, 1)));
  distance_matrix_free(with_inf);

  /* Fichiers invalides : tous doivent donner NULL, sans fuite mémoire. */
  expect_true("fichier absent -> NULL",
              distance_matrix_load_csv("absent.csv") == NULL);
  expect_true("chemin NULL -> NULL", distance_matrix_load_csv(NULL) == NULL);
  expect_true("save sur matrice NULL -> 0",
              distance_matrix_save_csv(NULL, path) == 0);

  write_file(path, "taille,2\nnodes,4,9\n0,1\n1,0\n");
  expect_true("en-tete size absent -> NULL",
              distance_matrix_load_csv(path) == NULL);
  write_file(path, "size,2\nnoeuds,4,9\n0,1\n1,0\n");
  expect_true("ligne nodes absente -> NULL",
              distance_matrix_load_csv(path) == NULL);
  write_file(path, "size,3\nnodes,4,9,1\n0,1,2\n1,0,3\n");
  expect_true("ligne manquante -> NULL",
              distance_matrix_load_csv(path) == NULL);
  write_file(path, "size,2\nnodes,4,9\n0,1,7\n1,0\n");
  expect_true("valeur en trop -> NULL",
              distance_matrix_load_csv(path) == NULL);
  write_file(path, "size,2\nnodes,4,9\n0,abc\n1,0\n");
  expect_true("valeur non numerique -> NULL",
              distance_matrix_load_csv(path) == NULL);

  remove(path);
}

/* Vérifie que les mauvais paramètres renvoient NULL ou NAN proprement au lieu de faire planter le programme. */
static void test_invalid(const Graph *graph) {
  printf("test_invalid\n");
  int bad_node[] = {0, 9999};
  expect_true("graphe NULL -> NULL",
              distance_matrix_compute(NULL, POINTS, 2) == NULL);
  expect_true("nodes NULL -> NULL",
              distance_matrix_compute(graph, NULL, 2) == NULL);
  expect_true("size 0 -> NULL",
              distance_matrix_compute(graph, POINTS, 0) == NULL);
  expect_true("noeud hors bornes -> NULL",
              distance_matrix_compute(graph, bad_node, 2) == NULL);
  expect_true("get sur NULL -> NAN", isnan(distance_matrix_get(NULL, 0, 0)));
  distance_matrix_free(NULL);
  expect_true("free(NULL) sans crash", 1);
}

int main(void) {
  Graph *graph = graph_load_csv("data/osm/lyon_500/edges.csv");
  if (graph == NULL) {
    printf("[FAIL] impossible de charger data/osm/lyon_500/edges.csv\n");
    return 1;
  }

  DistanceMatrix *m = distance_matrix_compute(graph, POINTS, POINT_COUNT);
  if (m == NULL) {
    printf("[FAIL] distance_matrix_compute a renvoye NULL\n");
    graph_free(graph);
    return 1;
  }

  test_shape(m);
  test_get_bounds(m);
  test_diagonal(m);
  test_symmetry(m);
  test_triangle(m);
  test_matches_dijkstra(graph, m);
  test_early_stop();
  test_csv(m);
  test_invalid(graph);

  distance_matrix_free(m);
  graph_free(graph);

  printf("=> %d checks, %d erreurs\n", check_count, error_count);
  return error_count == 0 ? 0 : 1;
}