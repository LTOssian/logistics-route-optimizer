# Visite du code

Document pour défendre le code devant le prof : savoir répondre à « montrez-moi où ça se passe » et « pourquoi l'avoir écrit comme ça ». Chacun doit pouvoir réexpliquer chaque partie, pas seulement la sienne.

Vocabulaire : [[glossaire]] · Questions sur les algorithmes : [[questions_jury]] · Cours détaillés : `documentation/graph.md`, `documentation/dijkstra.md` (sur `main`).

> [!info] Règle de mise à jour
> Quand une phase est mergée dans `main`, la personne qui l'a codée remplit sa section ci-dessous (fichiers, fonction clé, choix, tests, points faibles). On cite les **fonctions**, pas les numéros de ligne, qui bougent.

---

## 1. Vue d'ensemble

```
src/
├── graph.{h,c}      graphe en liste d'adjacence + lecture du CSV
├── heap.{h,c}       tas min binaire (clé = distance)
├── dijkstra.{h,c}   3 versions : naive O(V²), tas, query (arrêt à la cible)
├── rng.{h,c}        générateur aléatoire xorshift64 (graine fixe)
└── main.c           programme `lro` : path (tsp, cvrp à venir)
tests/
├── test_graph.c  test_heap.c  test_rng.c  test_dijkstra.c
├── test_oracle.c    compare Dijkstra à une recherche exhaustive
└── test_expected.c  compare à data/toy/toy1.expected
```

Qui dépend de qui :

```mermaid
graph LR
  main.c --> dijkstra
  main.c --> graph
  dijkstra --> heap
  dijkstra --> graph
  tests --> rng
```

`rng` n'est utilisé que par les tests pour l'instant ; il servira aux points de livraison et aux demandes (phases 4 à 6).

### Le trajet d'une commande

`./build/lro path data/toy/toy1.csv 0 4` :

1. `main` lit `argv[1]`, voit `path`, appelle `cmd_path`.
2. `cmd_path` convertit la source et la cible, charge le graphe avec `graph_load_csv`, vérifie que les nœuds existent.
3. `dijkstra_query(graph, 0, 4)` calcule les distances et s'arrête quand le nœud 4 sort du tas.
4. `dijkstra_path(result, 4, &length)` remonte `previous[]` de 4 jusqu'à 0 et renvoie le chemin dans l'ordre.
5. Affichage, puis libération dans l'ordre inverse : chemin, résultat, graphe.

Sortie :

```
distance = 7.00
settled = 3
chemin = 0 -> 3 -> 4
```

`settled = 3` : seuls 0, 3 et 4 ont été traités. Le nœud 1 (distance 8) n'a pas eu besoin de l'être, c'est l'intérêt de l'arrêt anticipé.

---

## 2. Le graphe (`src/graph.{h,c}`)

### Structures

```c
typedef struct Edge {
  int to;            // nœud d'arrivée
  double weight;     // mètres, >= 0
  struct Edge *next; // arête suivante du même nœud
} Edge;

typedef struct Graph {
  int node_count;
  int edge_count;
  Edge **adjacency;  // adjacency[u] = première arête qui part de u
} Graph;
```

`adjacency` est un tableau de `node_count` têtes de listes chaînées. Parcourir les voisins de `u` :

```c
for (Edge *e = g->adjacency[u]; e != NULL; e = e->next) { ... }
```

### Fonctions

| Fonction | Ce qu'elle fait | À savoir dire |
|---|---|---|
| `graph_new(n)` | alloue le graphe et `n` listes vides | `calloc` met toutes les têtes à `NULL`, donc les listes sont vides sans boucle |
| `graph_add_edge(g, u, v, w)` | ajoute `u → v` | insertion **en tête** de liste : `O(1)`, aucune traversée. Refuse un nœud hors bornes et un poids négatif (Dijkstra serait faux) |
| `graph_load_csv(path)` | lit le fichier | **deux passes** : la 1ʳᵉ cherche l'en-tête `n,m` pour savoir combien de nœuds allouer, la 2ᵉ ajoute les arêtes. À la fin, refuse le fichier si le nombre d'arêtes lues ≠ `m` |
| `graph_free(g)` | libère tout | chaque maillon, puis le tableau, puis la structure |

Comment `graph_load_csv` reconnaît les lignes : `sscanf(line, "%d,%d,%lf", ...) == 3` → une arête ; sinon, la première ligne à deux entiers → l'en-tête. Les lignes de texte (`n,m`, `u,v,weight`) ne correspondent à rien et sont ignorées.

### Choix à justifier

- **Liste d'adjacence plutôt que matrice** : un carrefour a 2 à 3 rues. Une matrice V × V de `double` pour 10 000 nœuds prend 800 Mo, presque entièrement remplis d'« infini ».
- **Liste chaînée plutôt que tableau d'arêtes trié (CSR)** : plus simple à écrire et à expliquer, ajout en `O(1)`. Le CSR serait plus rapide (mémoire contiguë), c'est une optimisation possible, pas nécessaire à notre taille.
- **Arêtes orientées dans la structure, graphe non orienté dans les faits** : on stocke chaque rue deux fois (`u → v` et `v → u`). La même structure permettrait les sens uniques plus tard. (Le commentaire de `graph.h` dit « orienté », à préciser : #17.)
- **Insertion en tête** : les voisins sont parcourus dans l'ordre inverse du fichier. Ça ne change pas les distances, mais si deux chemins ont la même longueur, c'est cet ordre qui décide lequel est renvoyé.

---

## 3. Le tas (`src/heap.{h,c}`)

Un tableau dont la case 0 contient toujours la plus petite clé. Pour la case `i` : père en `(i − 1) / 2`, fils en `2i + 1` et `2i + 2`.

```c
typedef struct HeapItem { double key; int node; } HeapItem;
typedef struct MinHeap  { HeapItem *items; int size; int capacity; } MinHeap;
```

| Fonction | Principe | Coût |
|---|---|---|
| `heap_push` | on ajoute à la fin, puis `sift_up` : tant que le père est plus grand, on échange | `O(log n)` |
| `heap_pop` | on prend la case 0, on met la dernière à sa place, puis `sift_down` : on échange avec le plus petit des fils tant qu'il est plus petit | `O(log n)` |
| `heap_is_empty`, `heap_free` | — | `O(1)` |

### Choix à justifier

- **Le tas grandit tout seul** (`realloc`, capacité × 2) : avec la lazy deletion, un même nœud peut être inséré plusieurs fois, donc on ne connaît pas la taille à l'avance. Doubler donne un coût moyen `O(1)` par insertion. Si `realloc` échoue, l'ancien tableau est gardé et le tas reste valide.
- **Pas de decrease-key** : il faudrait mémoriser la position de chaque nœud dans le tas et la mettre à jour à chaque échange. La lazy deletion évite tout ça (voir plus bas).

---

## 4. Dijkstra (`src/dijkstra.{h,c}`)

Trois fonctions publiques, un seul type de résultat :

```c
typedef struct DijkstraResult {
  int node_count;
  double *distances;  // distances[v] : meilleure distance connue depuis la source
  int *previous;      // previous[v] : nœud d'avant sur ce chemin, -1 si aucun
  int settled_count;  // nombre de nœuds traités (mesure du travail)
} DijkstraResult;
```

| Fonction | Choix du prochain nœud | Arrêt | Rôle |
|---|---|---|---|
| `dijkstra_naive` | parcours de tout le tableau, `O(V)` par étape → `O(V²)` | plus aucun nœud atteignable | V1, référence du benchmark |
| `dijkstra` | tas → `O((V + E) log V)` | tas vide | V2, sert à la matrice (phase 4) |
| `dijkstra_query` | tas | la **cible sort du tas** | `lro path`, comparaison équitable avec A\* |

`dijkstra` et `dijkstra_query` partagent la même fonction interne, `dijkstra_run(graph, source, target)`, avec `target = -1` pour « pas d'arrêt ».

### La boucle principale (version tas)

```c
while (heap_pop(heap, &item)) {
  int current = item.node;
  if (closed[current])        // entrée périmée : on l'ignore
    continue;
  closed[current] = 1;
  result->settled_count++;
  if (current == target)      // la distance de la cible est définitive
    break;
  for (Edge *edge = graph->adjacency[current]; edge != NULL; edge = edge->next) {
    double candidate = result->distances[current] + edge->weight;
    if (candidate < result->distances[edge->to]) {   // relâchement
      result->distances[edge->to] = candidate;
      result->previous[edge->to] = current;
      heap_push(heap, candidate, edge->to);          // réinsertion
    }
  }
}
```

À savoir expliquer ligne par ligne :

- **Lazy deletion** : quand la distance d'un nœud s'améliore, on le réinsère au lieu de modifier l'ancienne entrée. L'ancienne sortira plus tard du tas, alors que le nœud est déjà fermé : `if (closed[current]) continue;` la jette. Le tas peut contenir jusqu'à E + 1 entrées, d'où `O(E log E)`, ce qui revient à `O(E log V)` puisque E ≤ V².
- **Pourquoi on s'arrête au pop de la cible, pas quand on la découvre** : au moment où elle est découverte, un chemin plus court peut encore exister. Quand elle sort du tas, tous les nœuds restants sont au moins aussi loin, et les poids sont positifs : rien ne peut plus la raccourcir. C'est l'invariant de Dijkstra.
- **Après `dijkstra_query`**, seule `distances[target]` est garantie ; les autres distances peuvent être provisoires.
- **`settled_count`** compte le travail indépendamment de la machine. C'est la métrique comparée à A\*.

### Reconstruction du chemin : `dijkstra_path`

On remonte `previous[]` depuis la cible jusqu'à `-1` (la source). Deux passes : la première compte la longueur pour allouer le tableau exact, la seconde le remplit **de la fin vers le début**, pour qu'il soit dans l'ordre source → cible sans avoir à l'inverser. Cible inatteignable (`distances[target]` infinie) → `NULL`. Source = cible → chemin d'un seul nœud.

### Gestion de la mémoire et `goto fail`

`dijkstra_run` alloue cinq blocs : le résultat, `distances`, `previous`, `closed` et le tas. Si l'une échoue, ou si un `heap_push` échoue en cours de route, on saute à un seul bloc de nettoyage :

```c
fail:
  free(closed);
  heap_free(heap);
  dijkstra_result_free(result);
  return NULL;
```

C'est l'usage classique de `goto` en C (le noyau Linux fait pareil) : un seul point de sortie en erreur, au lieu de répéter les `free` à chaque test. Ça marche parce que `free(NULL)` ne fait rien. La règle : la fonction qui renvoie un `DijkstraResult *` le confie à l'appelant, qui doit appeler `dijkstra_result_free`.

---

## 5. Le générateur aléatoire (`src/rng.{h,c}`)

```c
x ^= x << 13;  x ^= x >> 7;  x ^= x << 17;
```

Xorshift64 (Marsaglia, 2003) : trois décalages et trois « ou exclusif » sur un entier 64 bits.

- **Pourquoi pas `rand()`** : sa suite dépend de la libc. La même graine donne d'autres points sur macOS et sous Linux, donc d'autres chiffres d'un ordinateur à l'autre. Avec notre générateur, la graine 42 donne partout les mêmes tirages.
- **La graine 0 est remplacée** par une constante : avec un état nul, xorshift ne produit que des zéros.
- `rng_int(rng, bound)` prend le reste de la division par `bound`. Le léger biais du modulo est négligeable pour nos petites bornes.
- `rng_double` garde 53 bits, la précision exacte d'un `double`, et divise par 2⁵³ : résultat dans [0, 1).

Points faibles connus : `rng_next` et `rng_double` ne vérifient pas `NULL` (#17).

---

## 6. Le programme `lro` (`src/main.c`)

- `main` regarde `argv[1]` et appelle `cmd_path`, `cmd_tsp` ou `cmd_cvrp`.
- `cmd_tsp` et `cmd_cvrp` sont des squelettes qui renvoient 2 : les phases 5 et 6 n'ont qu'à remplir leur fonction, sans conflit sur le reste du fichier.
- Codes de retour : 0 si tout va bien, 1 en cas d'erreur ou s'il n'y a pas de chemin, 2 si la commande n'est pas encore codée. `make test` vérifie aussi la sortie de `lro path` sur le graphe jouet.
- Point faible connu : les nœuds sont lus avec `atoi`, qui transforme `abc` en 0 sans erreur. À remplacer par `strtol` avant la démo (#17).

---

## 7. Comment on sait que c'est juste (`tests/`)

| Test | Ce qu'il vérifie |
|---|---|
| `test_graph` | création, ajout d'arêtes, refus des entrées invalides, lecture du CSV |
| `test_heap` | ordre des sorties, croissance, pop sur tas vide, 500 clés aléatoires avec doublons : sortie triée, chaque élément sort exactement une fois |
| `test_rng` | valeur connue, même graine → même suite, graine 0, bornes de `rng_int` et `rng_double` |
| `test_dijkstra` | les trois versions sur le graphe jouet, nœuds inatteignables, `dijkstra_query` (source = cible → 1 nœud traité), paramètres invalides |
| `test_expected` | distances **et** chemins de `data/toy/toy1.expected` (graphe du CLRS) |
| `test_oracle` | les trois versions contre une recherche exhaustive |

### L'oracle exhaustif

Une fonction récursive (`explore`) essaie **tous les chemins simples** depuis la source et garde le plus court vers chaque nœud. C'est lent (exponentiel) mais impossible à rater, donc on le limite à 8 nœuds. Se limiter aux chemins simples suffit : avec des poids positifs, repasser par un nœud ne raccourcit jamais un chemin.

On compare sur le graphe jouet, sur des cas pièges (arêtes parallèles, poids nul, nœud isolé) et sur **100 graphes aléatoires** de 2 à 7 nœuds, avec une graine fixe pour qu'un échec soit reproductible.

Les distances sont comparées avec une tolérance (`fabs(a − b) < 1e-9`) : deux additions faites dans un ordre différent peuvent différer au dernier bit.

### Harnais et outils

- **Harnais maison** : des fonctions `expect_int`, `expect_double`… qui affichent `[ok]` ou `[FAIL]` et comptent les erreurs. Le binaire renvoie 1 s'il y en a, et `make test` s'arrête au premier binaire en échec.
- **valgrind** : la CI l'exécute sur chaque binaire de test, avec `--error-exitcode=1`. Une fuite ou un accès mémoire invalide fait échouer la PR.
- **Compilation** : `-Wall -Wextra -Werror` : le moindre avertissement bloque la compilation.
- **Docker** : la CI et nos machines utilisent la même image (même gcc, même valgrind).

---

## 8. Questions probables sur le code

**« Pourquoi du C ? »** Pour maîtriser la mémoire et la structure des données : on voit exactement ce que coûte un graphe et un tas, sans bibliothèque qui le cache.

**« Que se passe-t-il si un `malloc` échoue ? »** Chaque allocation est testée. La fonction libère ce qu'elle a déjà alloué (`goto fail` dans Dijkstra) et renvoie `NULL`. Le programme affiche une erreur au lieu de planter.

**« Pourquoi `double` et pas `int` ? »** Les longueurs OSM sont en mètres avec décimales. Les arrondir pourrait rendre une rue plus courte que le vol d'oiseau, et A\* ne serait plus juste.

**« Pourquoi pas de decrease-key ? »** La lazy deletion donne la même complexité en `O(E log V)` avec un tas beaucoup plus simple à écrire. Le prix : le tas contient quelques entrées en trop, jetées à la sortie.

**« Comment savez-vous que votre Dijkstra est juste ? »** Trois niveaux : des résultats calculés à la main (CLRS), un oracle exhaustif sur 100 graphes aléatoires, et les trois versions qui doivent être d'accord entre elles.

**« Pourquoi garder la version `O(V²)` ? »** Pour le benchmark : montrer que la structure de données change l'ordre de grandeur sur les vrais graphes. Sur un graphe très dense, elle serait même la meilleure.

**« Qui libère la mémoire ? »** Celui qui reçoit le pointeur. Chaque `_new` ou fonction qui renvoie un résultat a son `_free`. `dijkstra_path` renvoie un tableau à libérer avec `free`. valgrind le vérifie à chaque PR.

**« Votre graphe est orienté ou pas ? »** La structure est orientée, nos données ne le sont pas : chaque rue est stockée dans les deux sens. Gérer les sens uniques ne demanderait que de changer l'export.

**« Que se passe-t-il pour un nœud inatteignable ? »** Sa distance reste `INFINITY`, `dijkstra_path` renvoie `NULL` et `lro` affiche « pas de chemin ». En pratique, on ne garde que la plus grande composante connexe de Lyon.

---

## 9. Phases suivantes (à remplir après chaque merge)

Pour chaque phase : fichiers, fonction clé avec un extrait court, choix à justifier, tests, points faibles connus.

### Phase 2 — A\* (#3)

*À remplir.* À couvrir au minimum : `graph_load_nodes_csv` et les coordonnées dans `Graph`, la formule de Haversine et le rayon de 6 371 009 m, la clé du tas `f = g + h`, l'arrêt au pop de la cible, l'absence de réouverture, les graphes de test admissibles par construction.

### Phase 3 — Données OSM (#4)

*À remplir.* À couvrir : `scripts/osm_to_csv.py` étape par étape (téléchargement, plus grande composante, `to_undirected`, renumérotation), le format des fichiers, l'export en pleine précision, ce que contient `meta.txt`.

### Phase 4 — Matrice de distances (#5)

*À remplir.* À couvrir : un Dijkstra par point, l'arrêt quand tous les points sont extraits, la taille (N+1) × (N+1), le stockage en tableau à une dimension.

### Phase 5 — TSP (#6)

*À remplir.* À couvrir : plus proche voisin, le mouvement 2-opt et l'inversion du segment, le calcul du gain sans recalculer toute la tournée, Held-Karp et sa table indexée par des ensembles en bits.

### Phase 6 — CVRP (#7)

*À remplir.* À couvrir : le calcul et le tri des économies, les conditions de fusion de deux tournées (extrémités, capacité), Or-opt, la fonction qui vérifie une solution.

### Phase 7 — Benchmarks (#8)

*À remplir.* À couvrir : la mesure du temps avec `CLOCK_MONOTONIC`, la médiane de 5 répétitions, la compilation en `-O2`, le format des résultats et le script de tracé.
