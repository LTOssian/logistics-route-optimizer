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
  int to;            /**< id du nœud d'arrivée (0 <= to < n). */
  double weight;     /**< coût de l'arête (mètres, >= 0). */
  struct Edge *next; /**< arête suivante dans la liste d'adjacence. */
} Edge;

/**
 * @struct Graph
 * @brief Graphe orienté pondéré.
 *
 * Le champ `adj` est un tableau de `n` têtes de listes :
 * `adj[i]` contient les arêtes qui partent du nœud `i`.
 */
typedef struct Graph {
  int n;      /**< nombre de nœuds. */
  int m;      /**< nombre d'arêtes. */
  Edge **adj; /**< adj[i] = tête des arêtes sortant de i. */
} Graph;

/**
 * @brief Alloue un graphe à `n` nœuds et zéro arête.
 *
 * @param n nombre de nœuds (n >= 1).
 * @return le graphe alloué, ou NULL si `n` est invalide ou si
 *         l'allocation échoue.
 * @note À libérer avec graph_free().
 */
Graph *graph_new(int n);

/**
 * @brief Ajoute l'arête orientée `u -> v` de poids `w`.
 *
 * @param g graphe cible (non NULL).
 * @param u nœud source (0 <= u < g->n).
 * @param v nœud destination (0 <= v < g->n).
 * @param w poids de l'arête (w >= 0).
 * @return 1 en cas de succès, 0 si un paramètre est invalide ou si
 *         l'allocation échoue (le graphe reste alors inchangé).
 * @note L'arête est insérée en tête de liste : coût O(1).
 */
int graph_add_edge(Graph *g, int u, int v, double w);

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
 * La première ligne composée de deux entiers fournit `n` et `m`.
 * Chaque ligne composée de trois nombres fournit une arête `u,v,weight`.
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
 * @param g graphe à libérer (NULL accepté).
 */
void graph_free(Graph *g);

#endif
