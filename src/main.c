#include "astar.h"
#include "dijkstra.h"
#include "graph.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

static void usage(void) {
  printf("usage : lro <commande> ...\n");
  printf("  path <graphe.csv> <source> <cible> [--astar <nodes.csv>]\n");
  printf("                                       plus court chemin\n");
  printf("  tsp                                  (phase 5)\n");
  printf("  cvrp                                 (phase 6)\n");
}

/* strtol et pas atoi : atoi renvoie 0 pour "abc" et 4 pour "4x", sans erreur. */
static int parse_node(const char *text, int *out) {
  char *end;
  errno = 0;
  long value = strtol(text, &end, 10);
  if (errno != 0 || end == text || *end != '\0' || value < INT_MIN ||
      value > INT_MAX)
    return 0;
  *out = (int)value;
  return 1;
}

/* lro path <graphe.csv> <source> <cible> [--astar <nodes.csv>] */
static int cmd_path(int argc, char **argv) {
  if (argc != 5 && argc != 7) {
    usage();
    return 1;
  }

  const char *nodes_path = NULL;
  if (argc == 7) {
    if (strcmp(argv[5], "--astar") != 0) {
      usage();
      return 1;
    }
    nodes_path = argv[6];
  }

  int source;
  int target;
  if (!parse_node(argv[3], &source) || !parse_node(argv[4], &target)) {
    fprintf(stderr, "erreur : source et cible doivent etre des entiers\n");
    return 1;
  }

  Graph *graph = graph_load_csv(argv[2]);
  if (graph == NULL) {
    fprintf(stderr, "erreur : impossible de charger '%s'\n", argv[2]);
    return 1;
  }

  if (source < 0 || source >= graph->node_count || target < 0 ||
      target >= graph->node_count) {
    fprintf(stderr, "erreur : un noeud est hors bornes (attendu : 0 a %d)\n",
            graph->node_count - 1);
    graph_free(graph);
    return 1;
  }

  if (nodes_path != NULL && !graph_load_nodes_csv(graph, nodes_path)) {
    fprintf(stderr, "erreur : impossible de charger '%s'\n", nodes_path);
    graph_free(graph);
    return 1;
  }

  DijkstraResult *result = (nodes_path != NULL)
                               ? astar(graph, source, target)
                               : dijkstra_query(graph, source, target);
  if (result == NULL) {
    fprintf(stderr, "erreur : le calcul a echoue\n");
    graph_free(graph);
    return 1;
  }

  int code = 0;
  int length = 0;
  int *path = dijkstra_path(result, target, &length);
  if (path == NULL) {
    printf("pas de chemin de %d vers %d\n", source, target);
    code = 1;
  } else {
    printf("distance = %.2f\n", result->distances[target]);
    printf("settled = %d\n", result->settled_count);
    printf("chemin = ");
    for (int i = 0; i < length; i++) {
      if (i > 0)
        printf(" -> ");
      printf("%d", path[i]);
    }
    printf("\n");
  }

  free(path);
  dijkstra_result_free(result);
  graph_free(graph);
  return code;
}

/* Squelettes : les phases 5 et 6 rempliront ces fonctions. */
static int cmd_tsp(void) {
  fprintf(stderr, "lro tsp : pas encore implemente (phase 5)\n");
  return 2;
}

static int cmd_cvrp(void) {
  fprintf(stderr, "lro cvrp : pas encore implemente (phase 6)\n");
  return 2;
}

int main(int argc, char **argv) {
  if (argc < 2) {
    usage();
    return 1;
  }
  if (strcmp(argv[1], "path") == 0)
    return cmd_path(argc, argv);
  if (strcmp(argv[1], "tsp") == 0)
    return cmd_tsp();
  if (strcmp(argv[1], "cvrp") == 0)
    return cmd_cvrp();

  fprintf(stderr, "commande inconnue : %s\n", argv[1]);
  usage();
  return 1;
}