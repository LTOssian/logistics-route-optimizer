#include "graph.h"

#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Alloue un graphe à `n` nœuds et zéro arête.
 *
 * Le tableau `adj` est initialisé à NULL avec `calloc`, ce qui garantit
 * que chaque liste d'adjacence est vide au départ.
 */
Graph* graph_new(int n) {
    if (n <= 0) return NULL;

    Graph* g = malloc(sizeof(Graph));
    if (g == NULL) return NULL;

    g->adj = calloc((size_t)n, sizeof(Edge*));
    if (g->adj == NULL) {
        free(g);
        return NULL;
    }

    g->n = n;
    g->m = 0;
    return g;
}

/**
 * @brief Ajoute l'arête orientée `u -> v` de poids `w`.
 *
 * L'arête est insérée en tête de la liste d'adjacence de `u` : aucune
 * traversée n'est nécessaire, le coût est donc O(1).
 */
int graph_add_edge(Graph* g, int u, int v, double w) {
    if (g == NULL) return 0;
    if (u < 0 || u >= g->n) return 0;
    if (v < 0 || v >= g->n) return 0;
    if (w < 0.0) return 0;

    Edge* e = malloc(sizeof(Edge));
    if (e == NULL) return 0;

    e->to     = v;
    e->weight = w;
    e->next   = g->adj[u];
    g->adj[u] = e;
    g->m++;
    return 1;
}

/**
 * @brief Libère le graphe et toutes ses arêtes.
 *
 * On libère d'abord chaque maillon de chaque liste d'adjacence, puis le
 * tableau de têtes, puis la structure elle-même.
 */
void graph_free(Graph* g) {
    if (g == NULL) return;

    for (int i = 0; i < g->n; i++) {
        Edge* e = g->adj[i];
        while (e != NULL) {
            Edge* next = e->next;
            free(e);
            e = next;
        }
    }
    free(g->adj);
    free(g);
}

/**
 * @brief Charge un graphe depuis un fichier CSV canonique.
 *
 * Le fichier est lu en deux passes :
 *  - passe 1 : trouver la première ligne à deux entiers (`n,m`) ;
 *  - passe 2 : créer le graphe puis ajouter chaque ligne à trois
 *    nombres (`u,v,weight`) comme une arête.
 *
 * Les lignes non numériques (en-têtes, commentaires, lignes vides) sont
 * ignorées. Le graphe n'est retourné que si le nombre d'arêtes lues
 * correspond au `m` annoncé.
 */
Graph* graph_load_csv(const char* path) {
    if (path == NULL) return NULL;

    FILE* f = fopen(path, "r");
    if (f == NULL) return NULL;

    char line[1024];
    int n = -1;
    int m = -1;

    // Passe 1 : trouver n et m.
    while (fgets(line, sizeof line, f) != NULL) {
        int a, b;
        double w;

        // Une ligne à trois nombres est une arête : on l'ignore ici.
        if (sscanf(line, "%d,%d,%lf", &a, &b, &w) == 3) {
            continue;
        }

        // Première ligne à deux entiers : n,m.
        if (n < 0 && sscanf(line, "%d,%d", &a, &b) == 2) {
            n = a;
            m = b;
        }
    }
    fclose(f);

    if (n <= 0 || m < 0) return NULL;

    Graph* g = graph_new(n);
    if (g == NULL) return NULL;

    // Passe 2 : lire les arêtes.
    f = fopen(path, "r");
    if (f == NULL) {
        graph_free(g);
        return NULL;
    }

    while (fgets(line, sizeof line, f) != NULL) {
        int u, v;
        double w;

        if (sscanf(line, "%d,%d,%lf", &u, &v, &w) == 3) {
            if (!graph_add_edge(g, u, v, w)) {
                fclose(f);
                graph_free(g);
                return NULL;
            }
        }
    }
    fclose(f);

    // Vérifie la cohérence entre l'en-tête et les arêtes lues.
    if (g->m != m) {
        graph_free(g);
        return NULL;
    }

    return g;
}
