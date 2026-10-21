---
tags:
  - algorithme
  - graphe
  - data-structure
  - pathfinding
---

# Graphes — Cours

> [!summary] Idée centrale
> Un **graphe** modélise des **relations** entre objets : des **nœuds** (sommets) reliés par des **arêtes**. C'est la structure derrière les réseaux sociaux, les cartes routières, les dépendances et les parcours.
>
> `G = (V, E)` : `V` = ensemble de sommets, `E` = ensemble d'arêtes.

---

## 1. Motivation

Beaucoup de problèmes se ramènent à « comment aller d'un point A à un point B ? » :

- GPS : plus court chemin entre deux adresses.
- Réseaux sociaux : qui est connecté à qui ?
- Compilation : dans quel ordre compiler les fichiers (dépendances) ?
- Logistique : tournée de livraison passant par plusieurs points.

Les structures linéaires (tableau, liste) ne suffisent pas : il faut représenter des **relations multiples**.

| Structure | Modélise |
|---|---|
| Tableau | une suite d'éléments |
| Liste chaînée | une suite avec insertions/suppressions |
| Hash map | une association clé → valeur |
| **Graphe** | des **relations** entre éléments |

---

## 2. Définitions

| Terme             | Définition                                                                 |
| ----------------- | -------------------------------------------------------------------------- |
| **Sommet** (nœud) | Un objet du graphe (`V`).                                                  |
| **Arête**         | Un lien entre deux sommets (`E`).                                          |
| **Orienté**       | L'arête a un sens : `u -> v` (sens unique).                                |
| **Non orienté**   | Le lien est symétrique : `u -- v`.                                         |
| **Pondéré**       | Chaque arête a un coût / poids (distance, temps, prix).                    |
| **Degré**         | Nombre d'arêtes incidentes à un sommet (sortantes + entrantes si orienté). |
| **Chemin**        | Suite de sommets reliés par des arêtes.                                    |
| **Cycle**         | Chemin qui revient à son point de départ.                                  |
| **Connexe**       | Tout sommet est atteignable depuis tout autre.                             |
| **DAG**           | Graphe orienté **sans cycle** (*Directed Acyclic Graph*).                  |

> [!note] Connexité d'un graphe orienté
> - **Faiblement connexe** : connexe si on ignore le sens des arêtes.
> - **Fortement connexe** : pour tout couple `(u, v)`, il existe un chemin `u -> v` **et** `v -> u`.
>
> Un graphe routier peut être faiblement connexe mais pas fortement (sens uniques).

---

## 3. Types de graphes

| Type | Exemple |
|---|---|
| **Non orienté** | amitiés, routes à double sens |
| **Orienté** | followers, sens uniques, dépendances |
| **Pondéré** | distances, temps de trajet, coûts |
| **DAG** | dépendances de compilation, graphe de tâches |
| **Biparti** | clients ↔ produits, étudiants ↔ cours |

---

## 4. Représentations

Deux représentations classiques :

|                     | Liste d'adjacence | Matrice d'adjacence |
| ------------------- | ----------------- | ------------------- |
| Mémoire             | `O(V + E)`        | `O(V²)`             |
| Voisins d'un sommet | `O(degré)`        | `O(V)`              |
| Test d'arête `u–v`  | `O(degré)`        | `O(1)`              |
| Graphe creux        | ✅ idéal           | ❌ gaspillage        |
| Graphe dense        | correct           | ✅                   |

> [!important] Choix du projet
> Les graphes routiers sont **creux** (peu d'arêtes par nœud) et **orientés** (sens uniques) → on utilise une **liste d'adjacence**.

### En C (voir `src/graph.h`)

```c
typedef struct Edge {
    int          to;      // nœud d'arrivée
    double       weight;  // coût de l'arête
    struct Edge* next;    // arête suivante
} Edge;

typedef struct Graph {
    int    n;     // nombre de nœuds
    Edge** adj;   // adj[i] = arêtes sortant de i
} Graph;
```

```
adj ──> [0] -> (to=1,w=10) -> (to=3,w=5) -> NULL
        [1] -> (to=2,w=1)  -> (to=3,w=2) -> NULL
        [2] -> (to=4,w=4)  -> NULL
        [3] -> (to=1,w=3)  -> (to=2,w=9) -> (to=4,w=2) -> NULL
        [4] -> (to=0,w=7)  -> (to=2,w=6) -> NULL
```

### Parcourir les voisins

```c
for (Edge* e = g->adj[u]; e != NULL; e = e->next) {
    int    v = e->to;
    double w = e->weight;
    // traiter l'arête u -> v
}
```

---

## 5. Parcours

### BFS — en largeur (file FIFO)

- Explore **niveau par niveau**.
- Trouve le **plus court chemin en nombre d'arêtes** dans un graphe **non pondéré**.
- Complexité `O(V + E)`.

```c
void bfs(const Graph* g, int src, int* visited) {
    int* queue = malloc(sizeof(int) * g->n);
    int head = 0, tail = 0;

    queue[tail++] = src;
    visited[src] = 1;

    while (head < tail) {
        int u = queue[head++];
        for (Edge* e = g->adj[u]; e != NULL; e = e->next) {
            if (!visited[e->to]) {
                visited[e->to] = 1;
                queue[tail++] = e->to;
            }
        }
    }
    free(queue);
}
```

### DFS — en profondeur (pile / récursion)

- Explore une branche à fond avant de revenir.
- Sert à : détecter des cycles, tri topologique, composantes connexes.
- Complexité `O(V + E)`.

```c
void dfs(const Graph* g, int u, int* visited) {
    visited[u] = 1;
    for (Edge* e = g->adj[u]; e != NULL; e = e->next) {
        if (!visited[e->to]) dfs(g, e->to, visited);
    }
}
```

---

## 6. Plus court chemin

| Algo | Contexte | Complexité |
|---|---|---|
| **BFS** | non pondéré (nb d'arêtes) | `O(V + E)` |
| **Dijkstra** | poids ≥ 0 | `O((V+E) log V)` (tas binaire) |
| **Bellman-Ford** | poids négatifs | `O(V·E)` |
| **A\*** | avec heuristique | dépend de `h` |

> [!note] Lien avec le projet
> Dijkstra et A\* sont les algorithmes du projet logistique. Voir `etudes/djikstra.md` et `etudes/a_star.md`.

### Dijkstra — intuition

On maintient pour chaque nœud la meilleure distance connue depuis la source :

1. `dist[source] = 0`, les autres `= +∞`.
2. Répéter : prendre le nœud non traité de **plus petite distance**, le marquer traité, puis **relâcher** ses arêtes.
3. Relâcher `u -> v` : si `dist[u] + w < dist[v]`, on améliore `dist[v]` et on mémorise `prev[v] = u`.

On reconstruit ensuite le chemin en remontant `prev`.

---

## 7. Complexité

| Opération | Liste d'adjacence |
|---|---|
| Ajouter une arête | `O(1)` (en tête) |
| Parcourir les voisins de `u` | `O(degré(u))` |
| BFS / DFS | `O(V + E)` |
| Dijkstra (tas binaire) | `O((V+E) log V)` |
| Dijkstra (scan linéaire) | `O(V²)` |
| Mémoire | `O(V + E)` |

---

## 8. Cas d'usage

> [!example] Cas d'usage (graphes)
> - **GPS / routage** : plus court chemin (Dijkstra, A\*).
> - **Réseaux sociaux** : amis, recommandations, influence.
> - **Web** : crawl, PageRank (graphe de liens).
> - **Dépendances** : compilation (Makefile), gestionnaires de paquets, tri topologique.
> - **Réseaux** : routage de paquets, topologie.
> - **Planification** : ordonnancement de tâches (DAG).
> - **Logistique** : tournées de livraison (TSP/VRP) sur un réseau routier.

---

## 9. Implémentation C du projet

| Fichier | Rôle |
|---|---|
| `src/graph.h` | structs `Edge` / `Graph` + prototypes documentés |
| `src/graph.c` | `graph_new`, `graph_add_edge`, `graph_load_csv`, `graph_free` |

### Cycle de vie

```c
Graph* g = graph_new(5);          // 5 nœuds, 0 arête
graph_add_edge(g, 0, 1, 10.0);    // 0 -> 1, poids 10
graph_add_edge(g, 0, 3, 5.0);     // 0 -> 3, poids 5
graph_free(g);                    // libère arêtes + tableau + struct
```

### Format CSV canonique

```
n,m
5,10
u,v,weight
0,1,10.0
0,3,5.0
...
```

- ligne 1 : en-tête `n,m` (ignorée) ;
- ligne 2 : `n` nœuds, `m` arêtes ;
- ligne 3 : en-tête `u,v,weight` (ignorée) ;
- lignes suivantes : arêtes **orientées**.

### Règles de gestion

- Aucun `exit()` : toute erreur renvoie `NULL` ou `0`.
- `graph_new` / `graph_free` sont symétriques.
- `graph_load_csv` vérifie que `g->m == m` (en-tête cohérent).
- Le graphe reste inchangé si `graph_add_edge` échoue.

---

## 10. Tests à écrire

1. `graph_new(5)` → `n=5`, `m=0`.
2. `graph_add_edge` valide → `m` incrémenté, voisin présent.
3. `graph_add_edge` invalide (`u` hors bornes, poids négatif) → `0`, `m` inchangé.
4. `graph_load_csv("data/toy/toy1.csv")` → `n=5`, `m=10`.
5. Degrés sortants : `0→2`, `1→2`, `2→1`, `3→3`, `4→2`.
6. `graph_free` → valgrind propre (pas de fuite).

---

## 11. Exercices

1. Écrire `int graph_out_degree(const Graph* g, int u)`.
2. Écrire `int graph_has_edge(const Graph* g, int u, int v)`.
3. Charger `toy1` et afficher tous les voisins de chaque nœud.
4. BFS depuis `0` : afficher l'ordre de visite.
5. Compter les composantes connexes (sur un graphe non orienté).
