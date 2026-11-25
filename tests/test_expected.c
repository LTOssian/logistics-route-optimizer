#include "dijkstra.h"
#include "graph.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

/* Transforme un chemin {0, 3, 1} en texte "0-3-1". */
static void path_to_text(const int *path, int length, char *out) {
  out[0] = '\0';
  for (int i = 0; i < length; i++) {
    char part[16];
    if (i == 0)
      snprintf(part, sizeof part, "%d", path[i]);
    else
      snprintf(part, sizeof part, "-%d", path[i]);
    strcat(out, part);
  }
}

/* Vérifie une ligne du fichier pour un résultat Dijkstra. */
static void check_line(const char *version, const DijkstraResult *result,
                       int node, double distance, const char *expected_text) {
  char label[64];
  char text[256];
  int length = 0;

  snprintf(label, sizeof label, "%s : distance vers %d", version, node);
  expect_int(label, fabs(result->distances[node] - distance) < 1e-9, 1);

  int *path = dijkstra_path(result, node, &length);
  snprintf(label, sizeof label, "%s : chemin vers %d", version, node);
  if (path == NULL) {
    expect_int(label, 0, 1);
    return;
  }
  path_to_text(path, length, text);
  expect_int(label, strcmp(text, expected_text) == 0, 1);
  free(path);
}

int main(void) {
  Graph *graph = graph_load_csv("data/toy/toy1.csv");
  FILE *file = fopen("data/toy/toy1.expected", "r");
  expect_int("toy1.csv charge", graph != NULL, 1);
  expect_int("toy1.expected ouvert", file != NULL, 1);

  if (graph != NULL && file != NULL) {
    DijkstraResult *naive = dijkstra_naive(graph, 0);
    DijkstraResult *heap = dijkstra(graph, 0);
    expect_int("resultats non NULL", naive != NULL && heap != NULL, 1);

    int lines = 0;
    char line[256];
    while (naive != NULL && heap != NULL && fgets(line, sizeof line, file)) {
      int node;
      double distance;
      char text[64];
      /* les lignes qui commencent par # ne correspondent pas : ignorées */
      if (sscanf(line, "%d,%lf,%63s", &node, &distance, text) != 3)
        continue;
      check_line("naive", naive, node, distance, text);
      check_line("tas", heap, node, distance, text);
      lines++;
    }
    expect_int("5 lignes lues", lines, 5);

    dijkstra_result_free(naive);
    dijkstra_result_free(heap);
  }

  if (file != NULL)
    fclose(file);
  graph_free(graph);

  printf("=> %d checks, %d erreurs\n", check_count, error_count);
  return (error_count == 0) ? 0 : 1;
}