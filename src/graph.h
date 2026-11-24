#ifndef GRAPH_H
#define GRAPH_H

/**
 * @file graph.h
 * @brief Graphe orienté pondéré représenté par une liste d'adjacence.
 *
 * Un graphe `G = (V, E)` est stocké sous forme de listes d'adjacence :
 * chaque nœud possède la liste de ses arêtes sortantes. Ce choix est
 * adapté aux graphes routiers, qui sont creux (peu d'arêtes par nœud).
 */

/**
 * @struct Edge
 * @brief Une arête sortante d'un nœud.
 *
 * Chaque arête appartient à la liste d'adjacence de son nœud source.
 * Elle stocke la destination, le poids et un lien vers l'arête suivante.
 */
typedef struct Edge {
  int to;            /**< id du nœud d'arrivée. */
  double weight;     /**< coût de l'arête (mètres, >= 0). */
  struct Edge *next; /**< arête suivante dans la liste d'adjacence. */
} Edge;

/**
 * @struct Graph
 * @brief Graphe orienté pondéré.
 *
 * Le champ `adjacency` est un tableau de `node_count` têtes de listes :
 * `adjacency[index]` contient les arêtes qui partent du nœud `index`.
 */
typedef struct Graph {
  int node_count;      /**< nombre de nœuds. */
  int edge_count;      /**< nombre d'arêtes. */
  Edge **adjacency;    /**< adjacency[index] = tête des arêtes sortant de index. */
} Graph;

/**
 * @brief Alloue un graphe à `node_count` nœuds et zéro arête.
 *
 * @param node_count nombre de nœuds (node_count >= 1).
 * @return le graphe alloué, ou NULL si `node_count` est invalide ou si
 *         l'allocation échoue.
 * @note À libérer avec graph_free().
 */
Graph *graph_new(int node_count);

/**
 * @brief Ajoute l'arête orientée `from -> to` de poids `weight`.
 *
 * @param graph graphe cible (non NULL).
 * @param from nœud source (0 <= from < graph->node_count).
 * @param to nœud destination (0 <= to < graph->node_count).
 * @param weight poids de l'arête (weight >= 0).
 * @return 1 en cas de succès, 0 si un paramètre est invalide ou si
 *         l'allocation échoue (le graphe reste alors inchangé).
 * @note L'arête est insérée en tête de liste : coût O(1).
 */
int graph_add_edge(Graph *graph, int from, int to, double weight);

/**
 * @brief Charge un graphe depuis un fichier CSV canonique.
 *
 * Format attendu (les lignes non numériques, comme les en-têtes ou les
 * commentaires, sont ignorées) :
 * @code
 * n,m
 * 5,10
 * u,v,weight
 * 0,1,10.0
 * 0,3,5.0
 * ...
 * @endcode
 *
 * La première ligne composée de deux entiers fournit le nombre de nœuds
 * et le nombre d'arêtes. Chaque ligne composée de trois nombres fournit
 * une arête `u,v,weight`.
 *
 * @param path chemin du fichier à lire.
 * @return le graphe chargé, ou NULL en cas d'erreur (fichier
 *         introuvable, format invalide, allocation échouée).
 * @note À libérer avec graph_free().
 */
Graph *graph_load_csv(const char *path);

/**
 * @brief Libère le graphe et toutes ses arêtes.
 *
 * @param graph graphe à libérer (NULL accepté).
 */
void graph_free(Graph *graph);

#endif
