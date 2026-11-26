#ifndef POINTS_H
#define POINTS_H
#include "graph.h"
#include <stdint.h>

/**
 * @file points.h
 * @brief Choix des points de livraison : le dépôt et les N clients.
 *
 * Le dépôt est le nœud le plus proche d'un centre (lat, lon) ; les clients
 * sont tirés au hasard parmi les autres nœuds, avec une graine fixe pour que
 * tout le monde obtienne les mêmes points.
 */

/**
 * @brief Renvoie le nœud le plus proche du point (lat, lon).
 *
 * Utilise une approximation plane (équirectangulaire), suffisante pour
 * comparer des distances à l'échelle d'une ville.
 *
 * @param graph graphe dont les coordonnées ont été chargées
 *              (graph_load_nodes_csv()).
 * @param lat latitude du centre, en degrés.
 * @param lon longitude du centre, en degrés.
 * @return l'id du nœud le plus proche, ou -1 si `graph` est NULL ou n'a pas
 *         de coordonnées.
 */
int nearest_node(const Graph *graph, double lat, double lon);

/**
 * @brief Tire `client_count` clients distincts, différents du dépôt.
 *
 * @param node_count nombre de nœuds du graphe.
 * @param depot id du nœud dépôt (0 <= depot < node_count).
 * @param client_count nombre de clients N (1 <= N <= node_count - 1).
 * @param seed graine du générateur (le projet utilise 42).
 * @return un tableau de N + 1 ids : case 0 = dépôt, cases 1..N = clients ;
 *         NULL si un paramètre est invalide ou si l'allocation échoue.
 * @note Le tableau doit être libéré avec free().
 */
int *pick_points(int node_count, int depot, int client_count, uint64_t seed);

#endif