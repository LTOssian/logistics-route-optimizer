#include "distance_matrix.h"
#include "dijkstra.h"
#include <math.h>
#include <stdlib.h>

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