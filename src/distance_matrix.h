#ifndef DISTANCE_MATRIX_H
#define DISTANCE_MATRIX_H
#include "graph.h"

/**
 * @brief Taille maximale (dépôt + clients) acceptée par
 *        distance_matrix_load_csv().
 *
 * Protège contre un fichier qui annoncerait une taille énorme : 10000 points
 * donnent 10^8 doubles, soit environ 800 Mo.
 */
#define DISTANCE_MATRIX_MAX_SIZE 10000

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
 *         La matrice est rendue exactement symétrique : pour i < j,
 *         d[i][j] = d[j][i] = min des deux valeurs calculées (les deux
 *         Dijkstra peuvent différer d'environ 1e-9 par arrondi).
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
 * @brief Écrit la matrice dans un fichier CSV.
 *
 * Format (une ligne d'en-tête, la liste des nœuds, puis une ligne par point) :
 * @code
 * size,3
 * nodes,12,124,59
 * 0,304.25,865.6
 * 304.25,0,1169.9
 * 865.6,1169.9,0
 * @endcode
 * Les nombres sont écrits avec 17 chiffres significatifs : à la relecture,
 * on retrouve exactement les mêmes doubles. Un point inatteignable est
 * écrit `inf`.
 *
 * @param matrix la matrice à écrire.
 * @param path chemin du fichier (écrasé s'il existe).
 * @return 1 en cas de succès, 0 en cas d'erreur.
 */
int distance_matrix_save_csv(const DistanceMatrix *matrix, const char *path);

/**
 * @brief Relit une matrice écrite par distance_matrix_save_csv().
 *
 * Permet de tester le TSP et le CVRP sans recharger les données OSM.
 *
 * @param path chemin du fichier.
 * @return la matrice, ou NULL si le fichier est introuvable, mal formé, ou
 *         si sa taille dépasse DISTANCE_MATRIX_MAX_SIZE.
 * @note À libérer avec distance_matrix_free().
 */
DistanceMatrix *distance_matrix_load_csv(const char *path);

/**
 * @brief Libère une matrice.
 *
 * @param matrix matrice à libérer (NULL accepté).
 */
void distance_matrix_free(DistanceMatrix *matrix);

#endif