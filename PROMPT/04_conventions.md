# 04 — Conventions de code

## Langage et compilation

- **C11** (`-std=c11`).
- Flags obligatoires : `-Wall -Wextra -Werror -g`.
- **Zéro warning** autorisé : `-Werror` transforme tout warning en erreur.
- Build via `make` (voir plus bas).

## Nommage

| Élément | Convention | Exemple |
|---|---|---|
| Fonctions | `snake_case` | `graph_add_edge` |
| Types (`typedef`) | `PascalCase` | `Graph`, `DijkstraResult` |
| Variables | `snake_case` | `current_dist` |
| Constantes / macros | `UPPER_CASE` | `INFINITY`, `MAX_CAPACITY` |
| Fichiers | `snake_case` | `distance_matrix.c` |
| Commentaires | **français** | `// relaxe l'arête (u -> v)` |

## Organisation des fichiers

```
src/
├── graph.h / graph.c
├── heap.h / heap.c
├── dijkstra.h / dijkstra.c
├── astar.h / astar.c
├── distance_matrix.h / distance_matrix.c
├── tsp.h / tsp.c
├── vrp.h / vrp.c
└── main.c            # CLI, seul endroit qui fait des printf/exit
tests/
├── test_graph.c
├── test_heap.c
└── test_dijkstra.c
```

- Un module = un `.h` (interface publique) + un `.c` (implémentation).
- **Header guards** :
  ```c
  #ifndef GRAPH_H
  #define GRAPH_H
  /* ... */
  #endif
  ```
- Les fonctions `static` sont privées au fichier `.c`.
- `main.c` reste **fin** : parsing des arguments, appels aux modules, affichage.

## Mémoire

- Chaque module expose une paire `*_new` / `*_free` (ou `*_create` / `*_destroy`).
- **Qui alloue, libère** : le module qui possède la structure fournit sa fonction de libération.
- Toute allocation est vérifiée (`NULL`).
- Pas de fuite : viser un `valgrind` propre (`--leak-check=full`).
- Ne jamais `free()` un pointeur non alloué (nœuds de pile, etc.).
- `realloc` : passer par une variable temporaire pour ne pas perdre l'ancien pointeur en cas d'échec.

## Gestion d'erreurs

- **Aucun `exit()` dans `src/` sauf `main.c`.**
- Les fonctions retournent :
  - `NULL` pour un pointeur en cas d'échec ;
  - un `int` de statut (`1` = succès, `0` = échec) quand une valeur de retour est déjà utilisée.
- `main.c` traduit l'échec en message + code de retour.
- Messages d'erreur sur `stderr`.

## Unités et valeurs

- Poids : `double`, en **mètres** (MVP).
- Distance inatteignable : `INFINITY` (`<math.h>`).
- `prev[source] = -1`.
- Comparaisons de flottants : préférer une tolérance (`fabs(a - b) < 1e-9`) quand c'est une égalité attendue.

## Format de données

### Graphe (CSV)

`data/.../<nom>.csv` :
```
n,m
u,v,weight
0,1,4.0
0,2,1.0
2,1,2.0
```
- `n` = nombre de nœuds, `m` = nombre d'arêtes.
- Arêtes **orientées**. Pour un graphe non orienté, exporter les deux sens.
- IDs internes denses `0..n-1`.

### Coordonnées (CSV)

`data/.../<nom>_nodes.csv` :
```
id,lat,lon
0,48.8566,2.3522
```
Nécessaire à l'heuristique Haversine de A\*.

## Tests

- Harnais maison (pas de framework externe au départ).
- Un fichier `tests/test_<module>.c` par module.
- Convention de sortie : `PASS`/`FAIL` par test + compteur d'erreurs.
- `make test` compile et exécute toute la suite ; **code de sortie ≠ 0** si un test échoue.
- Tests critiques :
  - tas : ordre des pops ;
  - graphe : nombre d'arêtes, voisins ;
  - Dijkstra : distances + reconstruction + oracle exhaustif (≤ 8 nœuds) ;
  - A\* : distance identique à Dijkstra ;
  - TSP/VRP : validité des tournées (visite unique, capacités).

## Makefile

Cibles minimales :
```makefile
CC      = gcc
CFLAGS  = -Wall -Wextra -Werror -std=c11 -g

all:      # compile l'exécutable principal
test:     # compile + exécute les tests
bench:    # (phase 7) compile + exécute les benchmarks
clean:    # supprime les .o et binaires
```

## Benchmarks

- Mesure : `clock_gettime(CLOCK_MONOTONIC)`.
- Répéter chaque mesure ≥ 5 fois, rapporter la **médiane** (et min/max).
- Documenter : CPU, OS, compilateur, flags, date.
- Sortie : CSV dans `benchmarks/results/`, plots via Python/matplotlib.

## Reproductibilité

- Toute partie aléatoire (demandes VRP, métaheuristiques) utilise une **graine fixe** documentée.
- Ne pas dépendre de l'ordre d'itération d'une hash map non triée pour les résultats.

## Git et artefacts

- Ne jamais committer : `*.o`, binaires, `build/`, `.obsidian/*`, `.DS_Store`.
- Un commit = une unité logique (voir `05_workflow_git.md`).

## Definition of Done (pour un module)

1. `.h` + `.c` avec header guard.
2. Fonctions documentées (commentaires français).
3. Tests unitaires présents et verts (`make test`).
4. Aucun warning (`-Werror`).
5. Pas de fuite mémoire (valgrind si disponible).
6. Commit dédié.
