---
tags:
  - algorithme
  - graphe
  - plus-court-chemin
  - dijkstra
---

# Dijkstra — Cours

> [!summary] Idée centrale
> Dijkstra calcule les **plus courts chemins depuis une source** dans un graphe **pondéré à poids positifs**. Il explore les nœuds par distance croissante : une fois qu'un nœud est choisi, sa distance est **définitive**.
>
> Complexité : `O(V²)` en version simple, `O((V+E) log V)` avec un tas binaire.

---

## 1. Le problème

On a un graphe orienté pondéré `G = (V, E)`, un poids `w(u, v) ≥ 0` par arête, et une source `s`.

On veut, pour chaque nœud `v` :

- la **distance minimale** `distances[node]` de `source` à `node` ;
- un **chemin** réalisant cette distance.

Applications : GPS, routage réseau, logistique, jeux, etc.

---

## 2. Intuition

Dijkstra est un algorithme **glouton** :

> À chaque étape, on développe le nœud **atteignable le plus proche** de la source.

Comme tous les poids sont positifs, aucun détour ne peut « raccourcir » un chemin déjà trouvé. On peut donc **figer** ce nœud dès qu'on le choisit.

C'est une **exploration en cercles concentriques** autour de la source.

```
        (5)
   A --------- B
   |           |
  (2)         (1)
   |           |
   S --------- C
        (10)
```

Depuis `S` : on découvre `A` (2), puis `B` (5), puis `C` (10) — dans l'ordre des distances.

---

## 3. Algorithme

```text
Dijkstra(graph, source):
    pour tout nœud index:
        distances[index] = +infini
        previous[index] = -1
    distances[source] = 0
    visited = ensemble vide

    répéter jusqu'à V fois:
        current = nœud NON visité de plus petite distance
        si aucun nœud atteignable: arrêter

        marquer current visité

        pour chaque arête current -> neighbor de poids weight:
            si distances[current] + weight < distances[neighbor]:
                distances[neighbor] = distances[current] + weight
                previous[neighbor] = current
```

Le tableau `previous` mémorise le prédécesseur sur le meilleur chemin connu : il sert à **reconstruire** le chemin.

---

## 4. Pourquoi ça marche

> [!important] Invariant glouton
> Quand on extrait le nœud `current` de plus petite distance non visité, `distances[current]` est **la distance minimale réelle** de `source` à `current`.

**Intuition de la preuve :** supposons qu'il existe un chemin plus court vers `u`. Ce chemin passerait par un nœud `x` non visité. Mais `x` serait plus proche que `u` (poids positifs), donc on l'aurait extrait avant `u`. Contradiction.

> [!warning] Condition indispensable : poids ≥ 0
> Avec un poids **négatif**, un chemin plus court pourrait apparaître **après** avoir figé un nœud. Dijkstra devient faux.
> Dans ce cas, utiliser **Bellman-Ford** (`O(V·E)`).

---

## 5. Structures de données

### Version simple — `O(V²)`

À chaque itération, on cherche le minimum par un **parcours linéaire** du tableau `distances`.

- Avantage : très simple, pas de structure auxiliaire.
- Inconvénient : lent sur de grands graphes.
- C'est la version du projet (`src/djikstra.c`), pour valider la correction.

### Version optimisée — `O((V+E) log V)`

On remplace la recherche linéaire par un **tas min** (binary heap) contenant `(distance, nœud)`.

- Extraire le minimum : `O(log V)`.
- Relâcher une arête : `O(log V)` (insertion dans le tas).
- Total : `O((V + E) log V)`.

> [!note] Lazy deletion
> Pas besoin de `decrease-key` : quand une distance s'améliore, on insère une **nouvelle** entrée `(distance, nœud)` dans le tas. Les anciennes sont ignorées au pop.

---

## 6. Complexité

| Version | Temps | Mémoire |
|---|---|---|
| Scan linéaire | `O(V²)` | `O(V)` |
| Tas binaire | `O((V + E) log V)` | `O(V)` |
| Tas de Fibonacci | `O(E + V log V)` | `O(V)` |

> [!note] Graphe routier
> Un graphe routier est **creux** : `E ≈ c·V` (quelques arêtes par nœud). Le tas binaire est donc nettement meilleur que `O(V²)` sur une ville.

---

## 7. Reconstruction du chemin

On remonte la chaîne `previous` depuis la cible jusqu'à la source, puis on inverse :

```text
reconstruire(previous, target):
    chemin = []
    node = target
    tant que node != -1:
        chemin.ajouter(node)
        node = previous[node]
    inverser(chemin)
    retourner chemin
```

- `previous[source] = -1` stoppe la remontée.
- Si `distances[target] = +infini`, il n'y a **pas de chemin**.

---

## 8. Cas limites

| Cas | Comportement attendu |
|---|---|
| **Source = cible** | distance `0`, chemin `[source]` |
| **Cible inatteignable** | `distances = INFINITY`, pas de chemin (`NULL`) |
| **Plusieurs chemins optimaux** | n'importe lequel est valide (départage arbitraire) |
| **Poids négatifs** | **interdit** → Dijkstra faux, utiliser Bellman-Ford |
| **Graphe non connexe** | certains nœuds restent `INFINITY` |
| **Source invalide** | erreur (`NULL`) |
| **Graphe vide / `NULL`** | erreur (`NULL`) |

---

## 9. Implémentation C du projet

| Fichier | Rôle |
|---|---|
| `src/djikstra.h` | struct `DijkstraResult` + prototypes documentés |
| `src/djikstra.c` | `dijkstra`, `dijkstra_path`, `dijkstra_result_free` |

### API

```c
typedef struct DijkstraResult {
    int     node_count;   // nombre de nœuds
    double *distances;    // distances[index] = distance minimale source -> index
    int    *previous;     // previous[index] = prédécesseur sur le chemin
} DijkstraResult;

DijkstraResult *dijkstra(const Graph *graph, int source);
int            *dijkstra_path(const DijkstraResult *result, int target, int *path_length);
void            dijkstra_result_free(DijkstraResult *result);
```

### Exemple d'utilisation

```c
#include "graph.h"
#include "djikstra.h"

Graph *graph = graph_load_csv("data/toy/toy1.csv");
DijkstraResult *result = dijkstra(graph, 0);

int path_length = 0;
int *path = dijkstra_path(result, 4, &path_length);   // chemin 0 -> 4

// path == [0, 3, 4], path_length == 3, result->distances[4] == 7.0

free(path);
dijkstra_result_free(result);
graph_free(graph);
```

### Règles de gestion

- Aucun `exit()` : toute erreur renvoie `NULL`.
- `dijkstra` alloue `distances`, `previous` et un tableau `visited` temporaire (libéré avant retour).
- `dijkstra_path` alloue le tableau retourné : **à libérer par l'appelant**.
- `dijkstra_result_free(NULL)` est sans effet.

---

## 10. Tests

Voir `tests/test_djisktra.c` :

1. **Cas simple** : détour plus court choisi (`0→1→2` = 2 au lieu de `0→2` = 3).
2. **Inatteignable** : nœud isolé → `INFINITY`, chemin `NULL`.
3. **Égalités** : deux chemins de même longueur → distance correcte, chemin valide.
4. **`toy1.csv`** : distances `[0, 8, 9, 5, 7]` + chemins exacts.
5. **Paramètres invalides** : `NULL`, source hors bornes → `NULL`.

---

## 11. Exercices

1. Remplacer le scan linéaire par un **tas min** et vérifier que les résultats de `toy1` sont identiques.
2. Compter le nombre de nœuds **settled** (extraits) et comparer avec A\*.
3. S'arrêter dès que la cible est extraite (recherche **single-target**).
4. Implémenter Bellman-Ford et tester un graphe avec un poids négatif.
5. Exporter les distances dans un CSV pour la future **matrice de distances**.

---

## Référence

- Dijkstra, E. W. (1959). *A note on two problems in connexion with graphs.* Numerische Mathematik, 1, 269–271. Voir `etudes/djikstra.md`.
