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

- **Liste d'adjacence plutôt que matrice** : un nœud a environ 3 rues. Pour nos 5 344 nœuds, une matrice V × V de `double` prendrait 228 Mo, presque entièrement remplis d'« infini », contre moins de 1 Mo en liste d'adjacence.
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

### Phase 3 — Données OSM (#4, PR #18)

**Fichiers** : `scripts/osm_to_csv.py` (Python, environ 200 lignes), `data/osm/lyon_<R>/{edges.csv, nodes.csv, meta.txt}`.

**Le trajet** (`main`) :

1. Refus si un dossier `lyon_<R>` existe déjà, sauf `--force` : les données sont gelées.
2. `download(R)` pour **chaque** taille avant d'écrire quoi que ce soit. Si Overpass plante au milieu, aucun fichier n'a bougé.
3. `build_tables(graph)` transforme le graphe OSMnx en tableaux.
4. `write_files` écrit les trois fichiers, puis `check` les relit.

**`download`** : `ox.graph_from_point((45.7578, 4.8320), dist=R, network_type="drive")`. Deux valeurs par défaut font le travail : `dist_type="bbox"` découpe un **carré** de demi-côté R (pas un disque), et `retain_all=False` ne garde que la plus grande composante connexe. Puis `ox.convert.to_undirected` fusionne les deux sens de chaque rue.

**`build_tables`**, le cœur :

```python
osm_ids = sorted(graph.nodes)                       # numérotation stable
index = {osm_id: i for i, osm_id in enumerate(osm_ids)}
...
floor = haversine_m(*coords[a], *coords[b]) + MARGIN_M
if length < floor:
    if floor - length > MAX_RAISE_M:                # 1e-6 m
        raise ExportError(...)                      # vraie erreur de données
    streets[(a, b)] = floor                         # simple arrondi : on remonte
```

- **Renumérotation** : les identifiants OSM sont de grands nombres irréguliers. On les trie et on les numérote de 0 à n−1, ce qui permet au C d'utiliser des tableaux `dist[i]`. Le tri rend le résultat indépendant de l'ordre interne de networkx.
- **Boucles retirées, rues parallèles fusionnées** (on garde la plus courte) : aucun plus court chemin ne change, et chaque paire n'apparaît qu'une fois. À 4000 m : 13 boucles, 70 rues parallèles.
- **Poids ≥ vol d'oiseau + 1e-9 m** : c'est la condition pour qu'A\* reste optimal. OSMnx calcule les longueurs avec la même formule, mais pas exactement les mêmes arrondis : à 4000 m, 420 rues sur 8 128 étaient sous le vol d'oiseau, d'au plus 6e-10 m ; avec la marge, 849 sont remontées, de 1,6e-9 m au plus. La marge de 1e-9 m couvre aussi l'écart entre notre Python et notre C (3e-14 m). Au-delà de 1e-6 m, ce n'est plus un arrondi : le script s'arrête.

**`haversine_m`** est écrit dans le même ordre d'opérations que la version C (`degrés * π / 180`, `s * s`), pour réduire l'écart entre les deux langages.

**`write_files`** : `repr(x)` écrit chaque flottant avec assez de chiffres pour être relu exactement, d'où la « pleine précision ». `meta.txt` contient l'heure d'exécution, la version d'OSMnx, le serveur Overpass, R, n, m et le détail des corrections.

**`check`** relit les fichiers écrits : `n` lignes dans `nodes.csv` avec des ids 0, 1, 2…, un en-tête `n,m` égal au contenu, et un graphe connexe reconstruit depuis `edges.csv` relu. Les erreurs passent par une exception, pas par `assert`, qui serait désactivé par `python -O`.

**Vérifié avec le C** (code de la phase 2) : les 4 graphes se chargent, 0 arête sous le Haversine du C, 0 violation de consistance `h(u) ≤ w(u,v) + h(v)` sur 50 cibles par graphe, et A\* donne les mêmes distances que Dijkstra sur 1 000 paires par graphe.

**Points faibles connus**
- Pas de test automatique du script : `check` en tient lieu. Un test C sur `lyon_500` dans `make test` arrive avec la phase 2.
- Les sens uniques sont ignorés (choix du plan) : une vraie camionnette ferait parfois des détours.
- Sans le cache local, une régénération télécharge la carte OSM du jour : nœuds et numérotation peuvent changer. C'est pour ça que les jeux sont committés et gelés.
- `run_at` est l'heure d'exécution du script, pas la date de la base OSM.

### Phase 4 — Matrice de distances (#5)


# Phase 4 — Matrice de distances

1. En une phrase

On choisit un **dépôt** (carrefour le plus proche de Bellecour) et **N clients** tirés au hasard (graine 42), puis on calcule la **distance routière réelle** entre chaque paire de points avec Dijkstra. Le résultat est rangé dans une matrice `(N+1) × (N+1)` qu'on peut enregistrer et relire en CSV.

2. Pourquoi une matrice ?

Le TSP et le CVRP ne raisonnent pas sur la carte. Ils ont seulement besoin de savoir « combien de mètres entre le point i et le point j ». Ces algorithmes demandent cette distance des milliers de fois. On la calcule donc **une seule fois** avec Dijkstra, puis chaque lecture coûte `O(1)`.

```
carte de Lyon (graphe)
   ├─ nearest_node()  → dépôt (nœud le plus proche de Bellecour)
   ├─ pick_points()   → [dépôt, client1, ..., clientN]
   ▼
distance_matrix_compute()  → d[i][j] en mètres
   ▼
TSP / CVRP (phases 5 et 6) : ne lisent plus que la matrice
```

Convention : **le dépôt est toujours à l'indice 0**, les clients aux indices `1..N`.

---
3. Fichiers modifiés et ajoutés

|Fichier|Statut|Contenu|
|---|---|---|
|`src/dijkstra.h` / `src/dijkstra.c`|modifié|ajout de `dijkstra_multi()`|
|`src/distance_matrix.h` / `src/distance_matrix.c`|**nouveau**|structure `DistanceMatrix`, calcul, accès, CSV, libération|
|`src/points.h` / `src/points.c`|**nouveau**|choix du dépôt et tirage des clients|
|`tests/test_distance_matrix.c`|**nouveau**|34 checks|
|`tests/test_points.c`|**nouveau**|15 checks|
|`Makefile`|modifié|compilation et lancement des 2 nouveaux tests|

`points` s'appuie sur les coordonnées ajoutées dans `graph` avec la fonction `graph_load_nodes_csv()`, champs `graph->lat` et `graph->lon`

---

4. Les fonctions, une par une

4.1 `dijkstra_multi()` — `src/dijkstra.c`

```c
DijkstraResult *dijkstra_multi(const Graph *graph, int source,
                               const int *targets, int target_count);
```

**Rôle :** un Dijkstra depuis `source` qui **s'arrête dès que toutes les cibles sont sorties du tas**, au lieu d'explorer toute la ville.

**Comment :** c'est le Dijkstra existant (tas binaire + lazy deletion), avec un seul ajout :

- un tableau `is_target[]` marque les cibles ;
- un compteur `remaining` contient le nombre de cibles **distinctes**, une cible en double n'étant comptée qu'une fois ;
- chaque fois qu'une cible est **extraite** du tas, on décrémente `remaining`. À 0, on s'arrête (`break`).

**Pourquoi s'arrêter à l'extraction et pas quand une cible est atteinte ?** Quand on atteint un nœud pour la première fois, sa distance est **provisoire** : un autre chemin peut encore l'améliorer. Elle ne devient **définitive** qu'au moment où le nœud sort du tas. S'arrêter plus tôt pourrait donner une distance fausse. C'est la même règle que `dijkstra_query()`, appliquée à plusieurs cibles.

**Gain mesuré :** sur `lyon_4000` (5 344 nœuds), avec 5 cibles, on ferme 4 598 nœuds au lieu de 5 344. Le gain grandit quand les points sont proches entre eux ou peu nombreux.

**Complexité :** `O((V + E) log V)` dans le pire cas, comme Dijkstra. En pratique, c'est moins grâce à l'arrêt anticipé.

**Cas particuliers :** une cible inatteignable garde la distance `INFINITY`. Des paramètres invalides donnent `NULL`.

---

4.2 `DistanceMatrix` — `src/distance_matrix.h`

```c
typedef struct DistanceMatrix {
  int size;     // N + 1 (dépôt + clients)
  int *nodes;   // nodes[i] = id du nœud du graphe ; nodes[0] = dépôt
  double *data; // size * size distances, ligne par ligne
} DistanceMatrix;
```

**Pourquoi un tableau à plat plutôt qu'un `double**` ?** On n'a qu'un seul `malloc` et un seul `free`, et les données sont contiguës en mémoire. La case `d[i][j]` se trouve à `data[i * size + j]` :

```
data = [ d00 d01 d02 | d10 d11 d12 | d20 d21 d22 ]
          ligne 0       ligne 1       ligne 2
```

**Pourquoi une matrice dense ?** Avec N entre 100 et 1 000, la matrice pèse au plus `1000² × 8 octets ≈ 8 Mo`, ce qui est raisonnable et très simple. C'est le choix noté dans le `PLAN.md`.

---

4.3 `distance_matrix_compute()`

```c
DistanceMatrix *distance_matrix_compute(const Graph *graph, const int *nodes, int size);
```

**Rôle :** remplir la matrice. **Une ligne = un `dijkstra_multi`** lancé depuis le point `i`, avec tous les points comme cibles. On recopie ensuite `distances[nodes[j]]` dans `d[i][j]`.

**Pourquoi depuis chaque point et pas seulement depuis le dépôt ?** Le TSP et le VRP ont besoin des distances **entre clients**, pas seulement dépôt → client.

**Complexité :** `size` Dijkstra, soit `O(N · (V + E) log V)`.

**Choix de conception :** la fonction reçoit la **liste des points** au lieu de la tirer elle-même. On peut donc la tester avec des points choisis à la main, et la réutiliser avec n'importe quelle liste.

---

4.4 `distance_matrix_get()` et `distance_matrix_free()`

- `get(m, i, j)` renvoie `d[i][j]`, ou `NAN` si `i` ou `j` sont hors bornes. On évite ainsi de lire n'importe où en mémoire.
- `free(m)` libère la structure et ses deux tableaux. Elle accepte `NULL`.

---

4.5 `distance_matrix_save_csv()` et `distance_matrix_load_csv()`

**Rôle :** enregistrer une matrice dans un fichier, puis la relire. L'issue le demande pour **pouvoir tester le TSP et le CVRP sans recharger les données OSM**.

**Format :**

```
size,4
nodes,12,124,59,86
0,993.28141895571821,513.48191737238733,561.25702557107206
993.2814189557181,0,1370.5171598902029,432.02439338464615
513.48191737238733,1370.5171598902027,0,938.49276650555692
561.25702557107206,432.0243933846462,938.49276650555714,0
```

(vrai export : `lyon_500`, dépôt + 3 clients, graine 42)

- **Pourquoi `%.17g` ?** 17 chiffres significatifs suffisent pour qu'à la relecture on retrouve **exactement** le même `double`. Le test le vérifie avec une égalité stricte.
- **Point inatteignable :** il est écrit `inf` et relu comme `INFINITY`.
- **Fichier mal formé :** en-tête absent, ligne manquante, valeur en trop ou texte au lieu d'un nombre donnent `NULL`, sans fuite mémoire.

---

4.6 `nearest_node()` — `src/points.c`

```c
int nearest_node(const Graph *graph, double lat, double lon);
```

**Rôle :** choisir le **dépôt**, c'est-à-dire le nœud du graphe le plus proche du centre (Bellecour : 45.7578, 4.832).

**Pourquoi le nœud le plus proche ?** Un point GPS n'est pas forcément sur une route. On prend donc le carrefour le plus proche. Un dépôt au centre donne des tournées équilibrées dans toutes les directions.

**Calcul :** on compare `dlat² + (dlon · cos(lat))²`.

- `cos(lat)` corrige le fait qu'à Lyon un degré de longitude (≈ 78 km) est plus court qu'un degré de latitude (≈ 111 km).
- On garde le carré, sans racine, parce qu'on compare seulement des distances entre elles.
- À l'échelle d'une ville, cette approximation plane suffit, pas besoin de Haversine.

**Résultat :** le dépôt est **le même carrefour sur les 4 cartes** (45.758396, 4.832482), avec un id différent selon le fichier : nœud 12 dans `lyon_500`, nœud 285 dans `lyon_4000`. On peut donc comparer les résultats entre tailles de graphe.

**Complexité :** `O(V)`, un seul parcours des nœuds. Sans coordonnées chargées, la fonction renvoie `-1`.

---

4.7 `pick_points()` — `src/points.c`

```c
int *pick_points(int node_count, int depot, int client_count, uint64_t seed);
```

**Rôle :** produire la liste `[dépôt, client1, …, clientN]`, avec des clients tirés au hasard parmi les autres nœuds.

**Garanties :**

- **aucun doublon** et **le dépôt n'est jamais un client**. Un tableau `taken[]` marque les nœuds déjà pris, et un nœud déjà pris est simplement retiré ;
- `N` doit vérifier `1 ≤ N ≤ node_count − 1`, sinon la fonction renvoie `NULL`. Sans cette vérification, la boucle de tirage ne s'arrêterait jamais.

**Pourquoi une graine fixe (42) ?** Le générateur `rng` (xorshift64) n'est pas vraiment aléatoire : une même graine donne **toujours la même suite**. Tout le monde obtient donc les mêmes clients, et les résultats et benchmarks sont **reproductibles**. C'est une décision du `PLAN.md`. On n'utilise pas `rand()`, dont le résultat dépend de la plateforme.

**Complexité :** environ `O(N)` en moyenne quand `N ≪ V`. Les tirages rejetés deviennent fréquents seulement si on prend presque tous les nœuds.

---

5. Tests — comment on sait que c'est juste

`make test` lance les deux nouveaux tests. La CI les passe aussi sous **valgrind**, qui ne signale aucune fuite mémoire.

| Critère de l'issue                               | Test                                                                      |
| ------------------------------------------------ | ------------------------------------------------------------------------- |
| Matrice `(N+1)×(N+1)`, dépôt à l'indice 0        | `test_shape`                                                              |
| Diagonale nulle                                  | `test_diagonal`                                                           |
| Symétrie (à une tolérance relative près)         | `test_symmetry` (graphe non orienté, tolérance `1e-9`)                    |
| Inégalité triangulaire                           | `test_triangle` : `d[i][k] ≤ d[i][j] + d[j][k]` pour tout i, j, k         |
| Cohérence avec des Dijkstra isolés               | `test_matches_dijkstra` : chaque case = Dijkstra complet                  |
| Arrêt quand toutes les cibles sont **extraites** | `test_early_stop` : moins de nœuds fermés, mêmes distances                |
| Export / import CSV                              | `test_csv` : aller-retour exact, `inf`, 5 fichiers invalides              |
| Tirage graine 42, sans doublon                   | `test_pick` : dépôt en case 0, pas de doublon, même graine = mêmes points |
| Dépôt = nœud le plus proche du centre            | `test_nearest`                                                            |

**Pourquoi une tolérance pour la symétrie ?** Les deux sens d'une rue ont le même poids, mais Dijkstra additionne les distances dans un ordre différent selon le sens, et les arrondis des `double` peuvent différer au dernier chiffre. Exemple réel : `d[0][1] = 993.28141895571821` et `d[1][0] = 993.2814189557181`.

**Pourquoi la symétrie est testée sur `lyon_500` et pas sur `toy1` ?** `toy1.csv` est **orienté** (pas d'arête 1→0), donc sa matrice n'est pas symétrique.

---

6. Questions probables à l'oral

**Q : Pourquoi N Dijkstra et pas un algorithme many-to-many ?** C'est simple et correct, avec une complexité `O(N · (V+E) log V)`. Le many-to-many avancé (par exemple avec les Contraction Hierarchies) est un bonus hors périmètre, comme décidé dans le `PLAN.md`.

**Q : Qu'est-ce que la lazy deletion ?** Quand la distance d'un nœud s'améliore, on insère une nouvelle entrée dans le tas au lieu de modifier l'ancienne. À l'extraction, on ignore les entrées d'un nœud déjà fermé. C'est plus simple qu'un `decrease-key`.

**Q : Que se passe-t-il si un client est inatteignable ?** Sa distance reste `INFINITY` dans la matrice, et elle est écrite `inf` dans le CSV. Les graphes de Lyon ne gardent que la plus grande composante connexe, donc ça n'arrive pas en pratique.

**Q : Combien de mémoire prend la matrice ?** `(N+1)² × 8 octets`, soit environ 80 Ko pour 100 clients et environ 8 Mo pour 1 000.

**Q : Pourquoi la matrice n'est-elle pas exactement symétrique ?** À cause des arrondis des nombres à virgule (voir la section 5). L'écart est de l'ordre de `1e-13` mètre.

**Q : Pourquoi le dépôt est-il à l'indice 0 ?** C'est une convention : le TSP et le CVRP savent toujours où est l'entrepôt sans avoir à le chercher.

**Q : Pourquoi une graine fixe ?** Pour la reproductibilité. Tout le monde, sur n'importe quelle machine, obtient les mêmes clients, donc les mêmes résultats à comparer.


### Phase 5 — TSP (#6)

*À remplir.* À couvrir : plus proche voisin, le mouvement 2-opt et l'inversion du segment, le calcul du gain sans recalculer toute la tournée, Held-Karp et sa table indexée par des ensembles en bits.

### Phase 6 — CVRP (#7)

*À remplir.* À couvrir : le calcul et le tri des économies, les conditions de fusion de deux tournées (extrémités, capacité), Or-opt, la fonction qui vérifie une solution.

### Phase 7 — Benchmarks (#8)

*À remplir.* À couvrir : la mesure du temps avec `CLOCK_MONOTONIC`, la médiane de 5 répétitions, la compilation en `-O2`, le format des résultats et le script de tracé.
