#include "graph.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

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

  graph->lat = NULL;
  graph->lon = NULL;

  return graph;
}

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


  free(graph->lat);
  free(graph->lon);

  free(graph->adjacency);
  free(graph);
}

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

int graph_set_coords(Graph *graph, int node, double lat, double lon) {
  if (graph == NULL)
    return 0;
  if (node < 0 || node >= graph->node_count)
    return 0;
  if (lat < -90.0 || lat > 90.0 || lon < -180.0 || lon > 180.0)
    return 0;

  if (graph->lat == NULL || graph->lon == NULL) {
    size_t count = (size_t)graph->node_count;
    double *new_lat = malloc(sizeof(double) * count);
    double *new_lon = malloc(sizeof(double) * count);
    if (new_lat == NULL || new_lon == NULL) {
      free(new_lat);
      free(new_lon);
      return 0;
    }
    for (size_t index = 0; index < count; index++) {
      new_lat[index] = NAN;
      new_lon[index] = NAN;
    }
    free(graph->lat); /* au cas où un seul des deux serait alloué */
    free(graph->lon);
    graph->lat = new_lat;
    graph->lon = new_lon;
  }

  graph->lat[node] = lat;
  graph->lon[node] = lon;
  return 1;
}

int graph_load_nodes_csv(Graph *graph, const char *path) {
  if (graph == NULL || path == NULL)
    return 0;

  FILE *file = fopen(path, "r");
  if (file == NULL)
    return 0;

  size_t count = (size_t)graph->node_count;
  double *lat = malloc(sizeof(double) * count);
  double *lon = malloc(sizeof(double) * count);
  int *seen = calloc(count, sizeof(int));
  if (lat == NULL || lon == NULL || seen == NULL) {
    free(lat);
    free(lon);
    free(seen);
    fclose(file);
    return 0;
  }

  char line[1024];
  int loaded = 0;
  int valid = 1;

  while (fgets(line, sizeof line, file) != NULL) {
    int id;
    double node_lat;
    double node_lon;

    /* en-tête, commentaires, lignes vides : ignorés */
    if (sscanf(line, "%d,%lf,%lf", &id, &node_lat, &node_lon) != 3)
      continue;

    if (id < 0 || id >= graph->node_count || seen[id] || node_lat < -90.0 ||
        node_lat > 90.0 || node_lon < -180.0 || node_lon > 180.0) {
      valid = 0;
      break;
    }
    lat[id] = node_lat;
    lon[id] = node_lon;
    seen[id] = 1;
    loaded++;
  }
  fclose(file);
  free(seen);

  if (!valid || loaded != graph->node_count) {
    free(lat);
    free(lon);
    return 0;
  }

  free(graph->lat);
  free(graph->lon);
  graph->lat = lat;
  graph->lon = lon;
  return 1;
}