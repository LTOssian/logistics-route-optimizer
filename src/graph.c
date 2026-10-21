#include "graph.h"

#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Alloue un graphe à `node_count` nœuds et zéro arête.
 *
 * Le tableau `adjacency` est initialisé à NULL avec `calloc`, ce qui
 * garantit que chaque liste d'adjacence est vide au départ.
 */
Graph *graph_new(int node_count) {
  if (node_count <= 0)
    return NULL;

  Graph *graph = malloc(sizeof(Graph));
  if (graph == NULL)
    return NULL;

  graph->adjacency = calloc((size_t)node_count, sizeof(Edge *));
  if (graph->adjacency == NULL) {
    free(graph);
    return NULL;
  }

  graph->node_count = node_count;
  graph->edge_count = 0;
  return graph;
}

/**
 * @brief Ajoute l'arête orientée `from -> to` de poids `weight`.
 *
 * L'arête est insérée en tête de la liste d'adjacence de `from` : aucune
 * traversée n'est nécessaire, le coût est donc O(1).
 */
int graph_add_edge(Graph *graph, int from, int to, double weight) {
  if (graph == NULL)
    return 0;
  if (from < 0 || from >= graph->node_count)
    return 0;
  if (to < 0 || to >= graph->node_count)
    return 0;
  if (weight < 0.0)
    return 0;

  Edge *edge = malloc(sizeof(Edge));
  if (edge == NULL)
    return 0;

  edge->to = to;
  edge->weight = weight;
  edge->next = graph->adjacency[from];
  graph->adjacency[from] = edge;
  graph->edge_count++;
  return 1;
}

/**
 * @brief Libère le graphe et toutes ses arêtes.
 *
 * On libère d'abord chaque maillon de chaque liste d'adjacence, puis le
 * tableau de têtes, puis la structure elle-même.
 */
void graph_free(Graph *graph) {
  if (graph == NULL)
    return;

  for (int index = 0; index < graph->node_count; index++) {
    Edge *edge = graph->adjacency[index];
    while (edge != NULL) {
      Edge *next_edge = edge->next;
      free(edge);
      edge = next_edge;
    }
  }
  free(graph->adjacency);
  free(graph);
}

/**
 * @brief Charge un graphe depuis un fichier CSV canonique.
 *
 * Le fichier est lu en deux passes :
 *  - passe 1 : trouver la première ligne à deux entiers (nombre de nœuds,
 *    nombre d'arêtes) ;
 *  - passe 2 : créer le graphe puis ajouter chaque ligne à trois nombres
 *    (from, to, weight) comme une arête.
 *
 * Les lignes non numériques (en-têtes, commentaires, lignes vides) sont
 * ignorées. Le graphe n'est retourné que si le nombre d'arêtes lues
 * correspond au nombre annoncé.
 */
Graph *graph_load_csv(const char *path) {
  if (path == NULL)
    return NULL;

  FILE *file = fopen(path, "r");
  if (file == NULL)
    return NULL;

  char line[1024];
  int node_count = -1;
  int edge_count = -1;

  // Passe 1 : trouver le nombre de nœuds et d'arêtes.
  while (fgets(line, sizeof line, file) != NULL) {
    int first;
    int second;
    double weight;

    // Une ligne à trois nombres est une arête : on l'ignore ici.
    if (sscanf(line, "%d,%d,%lf", &first, &second, &weight) == 3) {
      continue;
    }

    // Première ligne à deux entiers : nombre de nœuds et d'arêtes.
    if (node_count < 0 && sscanf(line, "%d,%d", &first, &second) == 2) {
      node_count = first;
      edge_count = second;
    }
  }
  fclose(file);

  if (node_count <= 0 || edge_count < 0)
    return NULL;

  Graph *graph = graph_new(node_count);
  if (graph == NULL)
    return NULL;

  // Passe 2 : lire les arêtes.
  file = fopen(path, "r");
  if (file == NULL) {
    graph_free(graph);
    return NULL;
  }

  while (fgets(line, sizeof line, file) != NULL) {
    int from;
    int to;
    double weight;

    if (sscanf(line, "%d,%d,%lf", &from, &to, &weight) == 3) {
      if (!graph_add_edge(graph, from, to, weight)) {
        fclose(file);
        graph_free(graph);
        return NULL;
      }
    }
  }
  fclose(file);

  // Vérifie la cohérence entre l'en-tête et les arêtes lues.
  if (graph->edge_count != edge_count) {
    graph_free(graph);
    return NULL;
  }

  return graph;
}
