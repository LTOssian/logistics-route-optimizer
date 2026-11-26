#ifndef DISTANCE_MATRIX_H
#define DISTANCE_MATRIX_H
#include "graph.h"

/**
 * @file distance_matrix.h
 * @brief Matrice des plus courtes distances entre un dépôt et des clients.
 *
 * La matrice est carrée, de taille `size` x `size` (size = N + 1) :
 * l'indice 0 est le dépôt, les indices 1..N sont les clients.
 * Elle sert de base au TSP (phase 5) et au CVRP (phase 6).
 */

/**
 * @struct DistanceMatrix
 * @brief Matrice dense de distances, stockée dans un tableau à plat.
 *
 * La distance du point i au point j se trouve dans `data[i * size + j]`
 * (utiliser distance_matrix_get()).
 */
typedef struct DistanceMatrix {
  int size;     /**< nombre de points (dépôt + clients). */
  int *nodes;   /**< nodes[i] = id du nœud du graphe ; nodes[0] = dépôt. */
  double *data; /**< size * size distances, ligne par ligne. */
} DistanceMatrix;

/**
 * @brief Calcule la matrice des distances entre les nœuds `nodes`.
 *
 * Lance un Dijkstra depuis chaque point, arrêté dès que tous les points
 * ont été extraits du tas (dijkstra_multi()). Complexité O(N (E log V)).
 *
 * @param graph graphe pondéré (poids >= 0).
 * @param nodes tableau de `size` ids de nœuds ; nodes[0] est le dépôt.
 * @param size nombre de points (size >= 1).
 * @return la matrice allouée, ou NULL si un paramètre est invalide ou si
 *         une allocation échoue. Un point inatteignable donne INFINITY.
 * @note À libérer avec distance_matrix_free().
 */
DistanceMatrix *distance_matrix_compute(const Graph *graph, const int *nodes,
                                        int size);

/**
 * @brief Renvoie la distance du point i au point j.
 *
 * @param matrix la matrice.
 * @param i indice du point de départ (0 <= i < size).
 * @param j indice du point d'arrivée (0 <= j < size).
 * @return la distance, ou NAN si un paramètre est invalide.
 */
double distance_matrix_get(const DistanceMatrix *matrix, int i, int j);

/**
 * @brief Libère une matrice.
 *
 * @param matrix matrice à libérer (NULL accepté).
 */
void distance_matrix_free(DistanceMatrix *matrix);

#endif