#include "points.h"

#include "rng.h"

#include <math.h>
#include <stdlib.h>

#define DEG_TO_RAD (3.14159265358979323846 / 180.0)

int nearest_node(const Graph *graph, double lat, double lon) {
  if (graph == NULL || graph->lat == NULL || graph->lon == NULL)
    return -1;

  double lon_scale = cos(lat * DEG_TO_RAD);

  int best = -1;
  double best_distance = INFINITY;
  for (int id = 0; id < graph->node_count; id++) {
    double dlat = graph->lat[id] - lat;
    double dlon = (graph->lon[id] - lon) * lon_scale;
    double distance = dlat * dlat + dlon * dlon;
    if (distance < best_distance) {
      best_distance = distance;
      best = id;
    }
  }
  return best;
}

int *pick_points(int node_count, int depot, int client_count, uint64_t seed) {
  if (node_count < 1 || depot < 0 || depot >= node_count)
    return NULL;
  if (client_count < 1 || client_count > node_count - 1)
    return NULL;

  int *points = malloc(sizeof(int) * (size_t)(client_count + 1));
  int *taken = calloc((size_t)node_count, sizeof(int));
  if (points == NULL || taken == NULL) {
    free(points);
    free(taken);
    return NULL;
  }

  Rng rng;
  rng_seed(&rng, seed);

  points[0] = depot;
  taken[depot] = 1;

  
  int count = 1;
  while (count <= client_count) {
    int candidate = rng_int(&rng, node_count);
    if (!taken[candidate]) {
      taken[candidate] = 1;
      points[count] = candidate;
      count++;
    }
  }

  free(taken);
  return points;
}