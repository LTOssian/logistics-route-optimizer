#include "astar.h"

#include "heap.h"

#include <math.h>
#include <stdlib.h>

#define PI 3.14159265358979323846

static double to_radians(double degrees) { return degrees * PI / 180.0; }

double haversine_m(double lat1, double lon1, double lat2, double lon2) {
  double phi1 = to_radians(lat1);
  double phi2 = to_radians(lat2);
  double delta_phi = to_radians(lat2 - lat1);
  double delta_lambda = to_radians(lon2 - lon1);

  double sin_phi = sin(delta_phi / 2.0);
  double sin_lambda = sin(delta_lambda / 2.0);
  double a = sin_phi * sin_phi + cos(phi1) * cos(phi2) * sin_lambda * sin_lambda;
  if (a > 1.0)
    a = 1.0; /* erreurs d'arrondi aux antipodes */

  return 2.0 * EARTH_RADIUS_M * asin(sqrt(a));
}

static double heuristic(const Graph *graph, int node, int target) {
  return haversine_m(graph->lat[node], graph->lon[node], graph->lat[target],
                     graph->lon[target]);
}

DijkstraResult *astar(const Graph *graph, int source, int target) {
  if (graph == NULL || graph->lat == NULL || graph->lon == NULL)
    return NULL;
  if (source < 0 || source >= graph->node_count)
    return NULL;
  if (target < 0 || target >= graph->node_count)
    return NULL;

  DijkstraResult *result = malloc(sizeof(DijkstraResult));
  if (result == NULL)
    return NULL;

  size_t count = (size_t)graph->node_count;
  result->node_count = graph->node_count;
  result->settled_count = 0;
  result->distances = malloc(sizeof(double) * count);
  result->previous = malloc(sizeof(int) * count);
  int *closed = calloc(count, sizeof(int));
  MinHeap *heap = heap_new(graph->node_count);
  HeapItem item;

  if (result->distances == NULL || result->previous == NULL ||
      closed == NULL || heap == NULL)
    goto fail;

  for (int index = 0; index < graph->node_count; index++) {
    result->distances[index] = INFINITY;
    result->previous[index] = -1;
  }
  result->distances[source] = 0.0;

  /* clé du tas = f = g + h ; g(source) = 0 */
  if (!heap_push(heap, heuristic(graph, source, target), source))
    goto fail;

  while (heap_pop(heap, &item)) {
    int current = item.node;

    /* entrée périmée (lazy deletion) : nœud déjà fermé */
    if (closed[current])
      continue;

    closed[current] = 1;
    result->settled_count++;

    /* arrêt au POP de la cible : seule façon de garantir l'optimalité */
    if (current == target)
      break;

    for (Edge *edge = graph->adjacency[current]; edge != NULL;
         edge = edge->next) {
      /* pas de réouverture : h consistante, le premier g est définitif */
      if (closed[edge->to])
        continue;

      double candidate = result->distances[current] + edge->weight;
      if (candidate < result->distances[edge->to]) {
        result->distances[edge->to] = candidate;
        result->previous[edge->to] = current;
        double f = candidate + heuristic(graph, edge->to, target);
        if (!heap_push(heap, f, edge->to))
          goto fail;
      }
    }
  }

  free(closed);
  heap_free(heap);
  return result;

fail:
  free(closed);
  heap_free(heap);
  dijkstra_result_free(result);
  return NULL;
}