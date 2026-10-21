---
tags:
  - algorithme
  - mathematiques
  - astar
  - pathfinding
---

# A* — Recherche de chemin dans un graphe

> [!question] Problématique
> Trouver un chemin à travers un graphe.

Nous avons tenté de résoudre cette problématique par **deux approches** :

- l'approche **mathématique** (exacte)
- l'approche **heuristique** (guidée)

| Approche | Principe | Chemin le moins coûteux garanti |
|---|---|---|
| **Mathématique** (Dijkstra, Bellman-Ford) | exploration exacte et uniforme | oui |
| **Heuristique** (A\*) | exploration guidée par `h(n)` | oui si `h` est **admissible**, non sinon |

## L'approche mathématique

Approche visant à examiner les **nœuds** d'un graphe pour établir le **chemin le moins coûteux**. On se soucie cependant moins de la **puissance de calcul** nécessaire pour lancer ces algorithmes.

## L'approche heuristique

Approche guidant la recherche grâce à une **fonction heuristique `h(n)`** qui **estime le coût restant** jusqu'au but (définie selon le contexte : distance euclidienne, Manhattan, etc.). Elle réduit le nombre de nœuds explorés.

> [!important] A* est optimal si l'heuristique est admissible
> Une heuristique **quelconque** ne garantit pas l'optimalité. En revanche, **A\* avec une heuristique admissible** garantit le chemin le moins coûteux.
>
> Une heuristique `h(n)` est **admissible** si elle ne surestime jamais le coût réel restant :
> `h(n) ≤ coût_vrai(n → but)` pour tout nœud `n`.

## Le cœur de A* : `f(n) = g(n) + h(n)`

| Terme | Signification |
|---|---|
| `g(n)` | coût **réel** accumulé depuis le départ jusqu'à `n` |
| `h(n)` | **estimation** du coût restant de `n` jusqu'au but |
| `f(n)` | **estimation** du coût total du chemin passant par `n` |

A* choisit toujours d'explorer le nœud de **plus petit `f(n)`**. C'est **Dijkstra + `h`** : avec `h = 0`, A* dégénère en Dijkstra.

### Admissible vs consistante

| Propriété | Définition | Conséquence |
|---|---|---|
| **Admissible** | `h(n) ≤ coût_vrai(n → but)` | garantit que le chemin trouvé est **optimal** |
| **Consistante** (monotone) | `h(n) ≤ coût(n → n') + h(n')` pour chaque arête | garantit l'optimalité **sans rouvrir** les nœuds déjà fermés |

- Une heuristique **consistante** est toujours **admissible** (la réciproque est fausse).
- Une heuristique **inadmissible** peut aller plus vite mais **perd la garantie d'optimalité** (Weighted A*).

---

# 1. Pseudo-code de l'algorithme

```text
A*(start, goal, h):
    open  ← tas min contenant (f(start), start)
    g[start] ← 0
    came_from[start] ← NULL
    closed ← ensemble vide

    tant que open n'est pas vide:
        n ← extraire le nœud de plus petit f

        si n == goal:
            retourner reconstruire_chemin(came_from, n)

        si n ∈ closed:           # entrée périmée (lazy deletion)
            continuer
        ajouter n à closed

        pour chaque voisin v de n:
            tentative_g ← g[n] + poids(n, v)

            si v n'a pas encore de g  OU  tentative_g < g[v]:
                g[v] ← tentative_g
                came_from[v] ← n
                f[v] ← tentative_g + h(v)
                insérer (f[v], v) dans open
```

> [!note] Idée générale
> On garde une **frontière** (open) des chemins à explorer, triée par `f`. À chaque tour, on développe le candidat le plus prometteur (le plus petit `f`). Les nœuds définitivement traités vont dans `closed`.

> [!tip] Lazy deletion
> Pas besoin de "decrease-key" dans le tas : quand `g[v]` s'améliore, on insère simplement une **nouvelle** entrée `(f[v], v)`. Les anciennes deviennent périmées et seront ignorées au pop (car `n ∈ closed`).

---

# 2. Les structures de données en C

En C, pas de `std::priority_queue` : il faut les écrire soi-même.

| Rôle | Structure | Remarque |
|---|---|---|
| **open set** | tas min (binary heap) de `(f, node)` | extraire le min en `O(log V)` |
| **closed set** | tableau de `bool` ou **hash set** | ids denses → tableau ; ids épars → hash map |
| **g-score** | tableau `float g[V]` ou hash map | `+∞`/absent = pas encore atteint |
| **came_from** | tableau `int parent[V]` ou hash map | pour reconstruire le chemin |

```c
typedef struct HeapItem {
    float f;
    int   node;
} HeapItem;

typedef struct MinHeap {
    HeapItem* items;
    int       size;
    int       capacity;
} MinHeap;
```

Pour les **ids épars** ou les grandes villes, un `float g[V]` n'est pas toujours possible : utilise une **hash map** (cf. le cours hash map) `id → g` et `id → parent`.

> [!note] Pourquoi un tas min ?
> On veut toujours le plus petit `f`. Un tableau trié coûte `O(V)` par insertion ; un tas min fait `O(log V)`. C'est ce qui donne la complexité `O(E log V)`.

---

# 3. Reconstruction du chemin

Une fois le but atteint, on remonte les parents jusqu'au départ, puis on inverse.

```text
reconstruire_chemin(came_from, n):
    chemin ← liste vide
    tant que n != NULL:
        chemin.ajouter(n)
        n ← came_from[n]
    inverser(chemin)
    retourner chemin
```

- `came_from[start] = NULL` stoppe la remontée.
- Le chemin retourné va du **départ vers le but** (après inversion).

---

# 4. Condition d'arrêt

> [!warning] On s'arrête quand le but est **extrait** (pop), pas quand il est découvert
> Si on s'arrête dès qu'on **insère** le but dans l'open, le chemin trouvé n'est pas forcément optimal : un autre chemin encore dans l'open pourrait être moins coûteux. La garantie d'optimalité vient du fait que, quand le but sort du tas, son `f` est le plus petit possible → son `g` est optimal (avec `h` admissible).

- **But inatteignable** : l'open se vide sans jamais extraire le but → pas de chemin.

---

# 5. Complexité

| Implémentation de l'open | Temps | Remarque |
|---|---|---|
| Tas binaire | `O(E log V)` | le choix usuel |
| Tableau + scan linéaire | `O(V²)` | simple mais lent sur grands graphes |
| Tas de Fibonacci | `O(E + V log V)` | théorique, rarement en pratique |

- **Espace** : `O(V)` pour `g`, `came_from`, `closed`, plus `O(V)` pour le tas.
- **Pire cas** = celui de Dijkstra (on explore tout le graphe).
- **En pratique**, A* explore **beaucoup moins** de nœuds grâce à `h` — c'est tout l'intérêt.
- Avec une heuristique **inadmissible**, la complexité peut se dégrader et l'optimalité est perdue.

---

# 6. Représentation du graphe

Un graphe routier est **creux** (peu d'arêtes par nœud) et **orienté** (sens uniques) → **liste d'adjacence**.

## `Edge` — une arête sortante

| Champ | Type | Rôle |
|---|---|---|
| `to` | `int` | id du nœud **d'arrivée** |
| `weight` | `double` | coût de l'arête (distance, temps, péage) |
| `next` | `Edge*` | arête suivante dans la liste d'adjacence |

`Edge` est un **maillon de LSC** : il ne stocke que la destination et le poids, **pas** la source (la source est le nœud dont la liste contient l'arête).

## `Graph` — le graphe entier

| Champ | Type | Rôle |
|---|---|---|
| `n` | `int` | nombre de nœuds |
| `adj` | `Edge**` | tableau de `n` têtes de listes : `adj[i]` = arêtes sortant de `i` |

```c
typedef struct Edge {
    int          to;
    double       weight;
    struct Edge* next;
} Edge;

typedef struct Graph {
    int     n;        // nombre de nœuds
    Edge**  adj;      // adj[i] = liste des arêtes sortant de i
} Graph;
```

```
adj ──> [0] -> (to=1,w=5) -> (to=2,w=3) -> NULL
        [1] -> (to=3,w=2) -> NULL
        [2] -> NULL
        [3] -> NULL
```

Exemple : depuis le nœud `0`, deux arêtes sortent (vers `1` et `2`).

## Parcourir les voisins

```c
for (Edge* e = g->adj[n]; e != NULL; e = e->next) {
    int    v = e->to;
    double w = e->weight;
    // relaxer (n -> v) avec le poids w
}
```

> [!note] Mémoire
> `adj` est un tableau de pointeurs alloué sur le tas ; chaque `Edge` aussi. Il faudra tout libérer (comme une LSC par nœud) à la fin.

> [!warning] Unités
> L'heuristique doit être exprimée dans la **même unité** que `g`. Une matrice d'adjacence `V×V` est trop coûteuse en mémoire pour des milliers de nœuds.

---

# 7. Exemples d'heuristiques

| Heuristique | Formule | Admissible quand… |
|---|---|---|
| **Euclidienne** | `√(dx² + dy²)` | déplacements libres dans le plan |
| **Manhattan** | `abs(dx) + abs(dy)` | grille à 4 directions (pas de diagonale) |
| **Haversine** | distance grand-cercle (lat/lon) | coût = **distance** ; borne inférieure des routes |

## Exemples numériques

A = (0, 0), B = (3, 4) :
- **Euclidienne** : `√(3² + 4²) = 5`
- **Manhattan** : `abs(3) + abs(4) = 7`

Paris (48.8566, 2.3522) → Lyon (45.7640, 4.8357) :
- **Haversine** ≈ `392 km` (grand cercle)
- route réelle ≈ `465 km` → l'heuristique **sous-estime** → **admissible**

> [!warning] L'admissibilité dépend des mouvements et de l'unité
> - **Manhattan** devient **inadmissible** si les déplacements **diagonaux** sont autorisés (elle surestime).
> - Si le coût est un **temps**, `h = distance / vitesse_max` reste admissible (on divise par la vitesse maximale possible).

---

# 8. Cas limites

| Cas                           | Comportement attendu                                                               |
| ----------------------------- | ---------------------------------------------------------------------------------- |
| `start == goal`               | retourner `[start]` immédiatement                                                  |
| but inatteignable             | open vide → "pas de chemin"                                                        |
| **poids négatifs**            | **interdit** pour A*/Dijkstra (utiliser Bellman-Ford)                              |
| plusieurs chemins de même `f` | départage arbitraire ; possible de préférer le plus grand `g` (plus proche du but) |
| voisin déjà dans l'open       | mettre à jour si `tentative_g < g[v]` (insérer une nouvelle entrée)                |
| voisin déjà dans le closed    | voir section 9 (rouvrir seulement si nécessaire)                                   |

---

# 9. Quand rouvrir un nœud

C'est la subtilité qui sépare une implémentation "qui marche" d'une implémentation **correcte**.

| Heuristique                                | Faut-il rouvrir un nœud fermé ?                                    |
| ------------------------------------------ | ------------------------------------------------------------------ |
| **Consistante** (monotone)                 | **non** : le premier `g` trouvé est optimal                        |
| **Admissible seulement** (pas consistante) | **oui, parfois** : un meilleur `g` peut apparaître après fermeture |

**Procédure si heuristique seulement admissible :**
```text
si tentative_g < g[v]:
    g[v] ← tentative_g
    came_from[v] ← n
    si v ∈ closed:
        retirer v de closed      # on le rouvre
    insérer (f[v], v) dans open
```

> [!important] Sans réouverture…
> …A* avec une heuristique seulement admissible peut retourner un chemin **non optimal**. La plupart des implémentations étudiantes utilisent des heuristiques **consistantes** (Euclidienne, Manhattan sur grille 4-directions) pour éviter ce problème.

---

## Ce que définit l'étude

L'étude définit un **graphe** et nous explique les modalités définissant un **algorithme viable** pour cette problématique.

> [!info] Référence
> Hart, Nilsson & Raphael (1968) — *A Formal Basis for the Heuristic Determination of Minimum Cost Paths*, l'article original de A*.
