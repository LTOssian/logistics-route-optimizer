#include "distance_matrix.h"
#include "dijkstra.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

DistanceMatrix *distance_matrix_compute(const Graph *graph, const int *nodes,
                                        int size) {
  if (graph == NULL || nodes == NULL || size < 1)
    return NULL;
  for (int i = 0; i < size; i++) {
    if (nodes[i] < 0 || nodes[i] >= graph->node_count)
      return NULL;
  }

  DistanceMatrix *matrix = malloc(sizeof(DistanceMatrix));
  if (matrix == NULL)
    return NULL;

  matrix->size = size;
  matrix->nodes = malloc(sizeof(int) * (size_t)size);
  matrix->data = malloc(sizeof(double) * (size_t)size * (size_t)size);
  if (matrix->nodes == NULL || matrix->data == NULL) {
    distance_matrix_free(matrix);
    return NULL;
  }

  for (int i = 0; i < size; i++)
    matrix->nodes[i] = nodes[i];

  for (int i = 0; i < size; i++) {
    DijkstraResult *result = dijkstra_multi(graph, nodes[i], nodes, size);
    if (result == NULL) {
      distance_matrix_free(matrix);
      return NULL;
    }

    for (int j = 0; j < size; j++)
      matrix->data[i * size + j] = result->distances[nodes[j]];

    dijkstra_result_free(result);
  }
  
  for (int i = 0; i < size; i++) {
    for (int j = i + 1; j < size; j++) {
      double a = matrix->data[i * size + j];
      double b = matrix->data[j * size + i];
      double d = (a < b) ? a : b;
      matrix->data[i * size + j] = d;
      matrix->data[j * size + i] = d;
    }
  }

  return matrix;
}

double distance_matrix_get(const DistanceMatrix *matrix, int i, int j) {
  if (matrix == NULL)
    return NAN;
  if (i < 0 || i >= matrix->size || j < 0 || j >= matrix->size)
    return NAN;
  return matrix->data[i * matrix->size + j];
}

void distance_matrix_free(DistanceMatrix *matrix) {
  if (matrix == NULL)
    return;

  free(matrix->nodes);
  free(matrix->data);
  free(matrix);
}

int distance_matrix_save_csv(const DistanceMatrix *matrix, const char *path) {
  if (matrix == NULL || path == NULL)
    return 0;

  FILE *file = fopen(path, "w");
  if (file == NULL)
    return 0;

  // L1 : La taille
  fprintf(file, "size,%d\n", matrix->size);

  // L2 : les ids des nœuds du graphe (dépôt en premier)
  fprintf(file, "nodes");
  for (int i = 0; i < matrix->size; i++)
    fprintf(file, ",%d", matrix->nodes[i]);
  fprintf(file, "\n");

  // 1 ligne par point avec valeurs séparées par des virgules
  for (int i = 0; i < matrix->size; i++) {
    for (int j = 0; j < matrix->size; j++) {
      if (j > 0)
        fprintf(file, ",");
      fprintf(file, "%.17g", matrix->data[i * matrix->size + j]);
    }
    fprintf(file, "\n");
  }

  return fclose(file) == 0;
}

DistanceMatrix *distance_matrix_load_csv(const char *path) {
  if (path == NULL)
    return NULL;

  FILE *file = fopen(path, "r");
  if (file == NULL)
    return NULL;

  // L1 : "size,N".
  int size;
  if (fscanf(file, " size,%d", &size) != 1 || size < 1 ||
      size > DISTANCE_MATRIX_MAX_SIZE) {
    fclose(file);
    return NULL;
  }

  DistanceMatrix *matrix = malloc(sizeof(DistanceMatrix));
  if (matrix == NULL) {
    fclose(file);
    return NULL;
  }
  matrix->size = size;
  matrix->nodes = malloc(sizeof(int) * (size_t)size);
  matrix->data = malloc(sizeof(double) * (size_t)size * (size_t)size);
  if (matrix->nodes == NULL || matrix->data == NULL)
    goto fail;

  // L2 : "nodes,a,b,c...". On lit le mot (5 lettres max) et on vérifie
  char tag[6];
  if (fscanf(file, " %5[a-z]", tag) != 1 || strcmp(tag, "nodes") != 0)
    goto fail;
  for (int i = 0; i < size; i++) {
    if (fscanf(file, ",%d", &matrix->nodes[i]) != 1)
      goto fail;
  }

  // size valeurs par ligne. La première valeur d'une
  // ligne n'a pas de virgule devant, les suivantes oui.
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      const char *format = (j == 0) ? " %lf" : ",%lf";
      if (fscanf(file, format, &matrix->data[i * size + j]) != 1)
        goto fail;
    }
  }

  char extra;
  if (fscanf(file, " %c", &extra) == 1)
    goto fail;

  fclose(file);
  return matrix;

fail:
  fclose(file);
  distance_matrix_free(matrix);
  return NULL;
}