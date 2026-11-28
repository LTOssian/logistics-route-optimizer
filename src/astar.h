#ifndef ASTAR_H
#define ASTAR_H

#include "dijkstra.h"
#include "graph.h"

/**
 * @file astar.h
 * @brief Plus court chemin A* avec heuristique Haversine.
 *
 * Même résultat que dijkstra_query() (distance optimale) mais en fermant
 * moins de nœuds. Nécessite les coordonnées du graphe (lat/lon).
 */

/** Rayon terrestre moyen en mètres (même valeur qu'OSMnx). */
#define EARTH_RADIUS_M 6371009.0

/**
 * @brief Distance grand-cercle entre deux points (formule de Haversine).
 *
 * @param lat1 latitude du point 1 (degrés).
 * @param lon1 longitude du point 1 (degrés).
 * @param lat2 latitude du point 2 (degrés).
 * @param lon2 longitude du point 2 (degrés).
 * @return la distance en mètres.
 */
double haversine_m(double lat1, double lon1, double lat2, double lon2);

/**
 * @brief Calcule le plus court chemin de `source` à `target` par A*.
 *
 * Priorité f = g + h avec h = Haversine(nœud, cible). Arrêt au pop de la
 * cible, pas de réouverture des nœuds fermés (h consistante). Les poids
 * doivent vérifier weight(u, v) >= haversine(u, v), sinon l'optimalité
 * n'est plus garantie.
 *
 * @param graph graphe avec coordonnées (graph->lat et graph->lon non NULL).
 * @param source nœud de départ.
 * @param target nœud d'arrivée.
 * @return un résultat compatible avec dijkstra_path() (distances, previous,
 *         settled_count), ou NULL si un paramètre est invalide, si le graphe
 *         n'a pas de coordonnées ou si une allocation échoue.
 * @note Seule distances[target] et la chaîne previous qui y mène sont
 *       définitives. À libérer avec dijkstra_result_free().
 */
DijkstraResult *astar(const Graph *graph, int source, int target);

#endif