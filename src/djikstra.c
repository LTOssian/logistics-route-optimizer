#include "djikstra.h"

#include <math.h>
#include <stdlib.h>

/**
 * @brief Calcule les plus courts chemins depuis `source` (Dijkstra O(V^2)).
 *
 * Principe :
 *  1. distances[source] = 0, les autres = +infini ;
 *  2. répéter V fois : choisir le nœud non traité de plus petite distance,
 *     le marquer traité, puis relâcher toutes ses arêtes sortantes ;
 *  3. s'arrêter si le minimum trouvé est +infini (nœuds restants
 *     inatteignables).
 *
 * La sélection linéaire du minimum donne une complexité O(V^2), suffisante
 * pour valider la correction avant d'introduire un tas binaire.
 */
DijkstraResult *dijkstra(const Graph *graph, int source) {
  if (graph == NULL)
    return NULL;
  if (source < 0 || source >= graph->node_count)
    return NULL;

  DijkstraResult *result = malloc(sizeof(DijkstraResult));
  if (result == NULL)
    return NULL;

  result->node_count = graph->node_count;
  result->distances = malloc(sizeof(double) * (size_t)graph->node_count);
  result->previous = malloc(sizeof(int) * (size_t)graph->node_count);
  int *visited = calloc((size_t)graph->node_count, sizeof(int));

  if (result->distances == NULL || result->previous == NULL ||
      visited == NULL) {
    free(result->distances);
    free(result->previous);
    free(visited);
    free(result);
    return NULL;
  }

  for (int index = 0; index < graph->node_count; index++) {
    result->distances[index] = INFINITY;
    result->previous[index] = -1;
  }
  result->distances[source] = 0.0;

  for (int step = 0; step < graph->node_count; step++) {
    // Sélection du nœud non traité de plus petite distance.
    int current = -1;
    double best_distance = INFINITY;

    for (int index = 0; index < graph->node_count; index++) {
      if (!visited[index] && result->distances[index] < best_distance) {
        best_distance = result->distances[index];
        current = index;
      }
    }

    // Aucun nœud atteignable restant : les autres sont inatteignables.
    if (current == -1)
      break;

    visited[current] = 1;

    // Relâchement des arêtes sortantes du nœud courant.
    for (Edge *edge = graph->adjacency[current]; edge != NULL;
         edge = edge->next) {
      double candidate = result->distances[current] + edge->weight;
      if (candidate < result->distances[edge->to]) {
        result->distances[edge->to] = candidate;
        result->previous[edge->to] = current;
      }
    }
  }

  free(visited);
  return result;
}

/**
 * @brief Reconstruit un plus court chemin jusqu'à `target`.
 *
 * On remonte la chaîne `previous` depuis la cible jusqu'à la source, puis
 * on remplit le tableau à l'envers pour obtenir l'ordre source -> cible.
 */
int *dijkstra_path(const DijkstraResult *result, int target, int *path_length) {
  if (path_length != NULL)
    *path_length = 0;
  if (result == NULL)
    return NULL;
  if (target < 0 || target >= result->node_count)
    return NULL;
  if (isinf(result->distances[target]))
    return NULL;

  // Compter la longueur du chemin.
  int length = 0;
  for (int node = target; node != -1; node = result->previous[node]) {
    length++;
  }

  int *path = malloc(sizeof(int) * (size_t)length);
  if (path == NULL)
    return NULL;

  // Remplir à l'envers : target, ..., source.
  int position = length - 1;
  for (int node = target; node != -1; node = result->previous[node]) {
    path[position--] = node;
  }

  if (path_length != NULL)
    *path_length = length;
  return path;
}

/**
 * @brief Libère un résultat de Dijkstra.
 */
void dijkstra_result_free(DijkstraResult *result) {
  if (result == NULL)
    return;

  free(result->distances);
  free(result->previous);
  free(result);
}
