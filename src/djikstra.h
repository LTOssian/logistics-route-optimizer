#ifndef DJIKSTRA_H
#define DJIKSTRA_H

#include "graph.h"

/**
 * @file djikstra.h
 * @brief Plus courts chemins depuis une source (algorithme de Dijkstra).
 */

/**
 * @struct DijkstraResult
 * @brief Résultat d'un calcul de plus courts chemins.
 *
 * Contient, pour chaque nœud, sa distance minimale depuis la source et
 * le nœud précédent sur un plus court chemin.
 */
typedef struct DijkstraResult {
  int node_count;       /**< nombre de nœuds. */
  double *distances;    /**< distances[index] = distance minimale source -> index. */
  int *previous;        /**< previous[index] = nœud précédent, -1 pour la source. */
} DijkstraResult;

/**
 * @brief Calcule les plus courts chemins depuis `source`.
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
DijkstraResult *dijkstra(const Graph *graph, int source);

/**
 * @brief Reconstruit un plus court chemin de la source jusqu'à `target`.
 *
 * @param result résultat d'un appel à dijkstra().
 * @param target nœud d'arrivée (0 <= target < result->node_count).
 * @param path_length si non NULL, reçoit la longueur du chemin (nombre de nœuds).
 * @return un tableau alloué de `path_length` nœuds (source -> target), ou NULL
 *         si la cible est invalide ou inatteignable.
 * @note Le tableau retourné doit être libéré par l'appelant avec free().
 */
int *dijkstra_path(const DijkstraResult *result, int target, int *path_length);

/**
 * @brief Libère un résultat de Dijkstra.
 *
 * @param result résultat à libérer (NULL accepté).
 */
void dijkstra_result_free(DijkstraResult *result);

#endif
