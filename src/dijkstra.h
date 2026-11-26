#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include "graph.h"

/**
 * @file dijkstra.h
 * @brief Plus courts chemins depuis une source (algorithme de Dijkstra).
 *
 * Quatre variantes :
 *  - dijkstra_naive() : scan linéaire, O(V^2) (référence pour le benchmark) ;
 *  - dijkstra()       : tas binaire + lazy deletion, O((V + E) log V) ;
 *  - dijkstra_query() : comme dijkstra(), mais s'arrête quand la cible est
 *                       extraite du tas ;
 *  - dijkstra_multi() : comme dijkstra(), mais s'arrête quand toutes les
 *                       cibles ont été extraites du tas.
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
  int settled_count;    /**< nombre de nœuds extraits et fermés (settled). */
} DijkstraResult;

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
DijkstraResult *dijkstra_naive(const Graph *graph, int source);

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
DijkstraResult *dijkstra(const Graph *graph, int source);

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
DijkstraResult *dijkstra_query(const Graph *graph, int source, int target);

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
int *dijkstra_path(const DijkstraResult *result, int target, int *path_length);

/**
 * @brief Libère un résultat de Dijkstra.
 *
 * @param result résultat à libérer (NULL accepté).
 */
void dijkstra_result_free(DijkstraResult *result);

/**
 * @brief Plus courts chemins depuis `source`, arrêt quand toutes les cibles
 *        sont extraites du tas.
 *
 * Seules les distances vers les nœuds de `targets` sont garanties définitives.
 *
 * @param graph graphe pondéré (poids >= 0).
 * @param source nœud de départ.
 * @param targets tableau de `target_count` nœuds cibles.
 * @param target_count nombre de cibles (>= 1).
 * @return le résultat alloué, ou NULL en cas d'erreur.
 * @note À libérer avec dijkstra_result_free().
 */
DijkstraResult *dijkstra_multi(const Graph *graph, int source,
                               const int *targets, int target_count);

#endif