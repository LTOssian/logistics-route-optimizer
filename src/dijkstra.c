#include "dijkstra.h"

#include "heap.h"

#include <math.h>
#include <stdlib.h>

/**
 * @brief Calcule les plus courts chemins depuis `source` (version naïve).
 *
 * Implémentation simple en `O(V^2)` : à chaque itération, on sélectionne
 * par un parcours linéaire le nœud non traité de plus petite distance,
 * puis on relâche ses arêtes sortantes.
 *
 * @param graph graphe orienté pondéré (poids >= 0).
 * @param source nœud de départ (0 <= source < graph->node_count).
 * @return le résultat alloué, ou NULL si un paramètre est invalide ou si
 *         une allocation échoue.
 * @note À libérer avec dijkstra_result_free().
 */
DijkstraResult *dijkstra_naive(const Graph *graph, int source) {
  if (graph == NULL)
    return NULL;
  if (source < 0 || source >= graph->node_count)
    return NULL;

  DijkstraResult *result = malloc(sizeof(DijkstraResult));
  if (result == NULL)
    return NULL;

  result->node_count = graph->node_count;
  result->settled_count = 0;
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
    result->settled_count++;

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

/* ------------------------------------------------------------------ */
/* Version tas + lazy deletion                                        */
/* ------------------------------------------------------------------ */

/**
 * Fonction interne commune à dijkstra() et dijkstra_query().
 * target = -1 : pas d'arrêt anticipé (on traite tous les nœuds atteignables).
 * target >= 0 : on s'arrête quand target sort du tas.
 */
static DijkstraResult *dijkstra_run(const Graph *graph, int source,
                                    int target) {
  if (graph == NULL)
    return NULL;
  if (source < 0 || source >= graph->node_count)
    return NULL;
  if (target < -1 || target >= graph->node_count)
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
      closed == NULL || heap == NULL) {
    goto fail;
  }

  for (int index = 0; index < graph->node_count; index++) {
    result->distances[index] = INFINITY;
    result->previous[index] = -1;
  }
  result->distances[source] = 0.0;

  if (!heap_push(heap, 0.0, source))
    goto fail;

  while (heap_pop(heap, &item)) {
    int current = item.node;

    // Entrée périmée (lazy deletion) : ce nœud est déjà fermé.
    if (closed[current])
      continue;

    closed[current] = 1;
    result->settled_count++;

    // Arrêt au POP de la cible : sa distance est alors définitive.
    if (current == target)
      break;

    for (Edge *edge = graph->adjacency[current]; edge != NULL;
         edge = edge->next) {
      double candidate = result->distances[current] + edge->weight;
      if (candidate < result->distances[edge->to]) {
        result->distances[edge->to] = candidate;
        result->previous[edge->to] = current;
        if (!heap_push(heap, candidate, edge->to))
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
  dijkstra_result_free(result); // free(NULL) est sans danger
  return NULL;
}

/**
 * @brief Calcule les plus courts chemins depuis `source` (tas + lazy deletion).
 *
 * Complexité `O((V + E) log V)`. Quand une distance s'améliore, on insère une
 * nouvelle entrée dans le tas ; les entrées périmées sont ignorées à
 * l'extraction.
 *
 * @param graph graphe orienté pondéré (poids >= 0).
 * @param source nœud de départ (0 <= source < graph->node_count).
 * @return le résultat alloué, ou NULL si un paramètre est invalide ou si
 *         une allocation échoue.
 * @note À libérer avec dijkstra_result_free().
 */
DijkstraResult *dijkstra(const Graph *graph, int source) {
  return dijkstra_run(graph, source, -1);
}

/**
 * @brief Calcule le plus court chemin de `source` à `target` (arrêt anticipé).
 *
 * Identique à dijkstra(), mais la recherche s'arrête dès que `target` est
 * extraite du tas. Seule `distances[target]` (et la chaîne `previous` qui y
 * mène) est garantie définitive ; les autres nœuds peuvent avoir une
 * distance provisoire. `settled_count` compte les nœuds fermés jusqu'à cet
 * arrêt, ce qui permet une comparaison équitable avec A*.
 *
 * @param graph graphe orienté pondéré (poids >= 0).
 * @param source nœud de départ (0 <= source < graph->node_count).
 * @param target nœud d'arrivée (0 <= target < graph->node_count).
 * @return le résultat alloué, ou NULL si un paramètre est invalide ou si
 *         une allocation échoue.
 * @note À libérer avec dijkstra_result_free().
 */
DijkstraResult *dijkstra_query(const Graph *graph, int source, int target) {
  if (target < 0)
    return NULL;
  return dijkstra_run(graph, source, target);
}

/**
 * @brief Reconstruit un plus court chemin de la source jusqu'à `target`.
 *
 * @param result résultat d'un appel à dijkstra(), dijkstra_naive() ou
 *               dijkstra_query().
 * @param target nœud d'arrivée (0 <= target < result->node_count).
 * @param path_length si non NULL, reçoit la longueur du chemin (nombre de nœuds).
 * @return un tableau alloué de `path_length` nœuds (source -> target), ou NULL
 *         si la cible est invalide ou inatteignable.
 * @note Le tableau retourné doit être libéré par l'appelant avec free().
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
 *
 * @param result résultat à libérer (NULL accepté).
 */
void dijkstra_result_free(DijkstraResult *result) {
  if (result == NULL)
    return;

  free(result->distances);
  free(result->previous);
  free(result);
}