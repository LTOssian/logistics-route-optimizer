# 02 — Décisions techniques

> [!important] Statut
> Ces décisions sont **validées par le groupe**. Ne pas les modifier sans en discuter. Voir les « points ouverts » en bas.

## Décisions transverses

| Décision | Choix | Pourquoi |
|---|---|---|
| Type des poids | `double` | distances OSM en mètres, futurs coûts « temps » ; évite l'accumulation d'erreurs d'arrondi |
| IDs de nœuds | IDs internes denses `0..n-1` | `dist[]`/`prev[]` sont des tableaux en `O(1)` ; le mapping OSM→interne se fait en Python |
| Représentation du graphe | liste d'adjacence | graphe routier creux ; la matrice `V×V` gaspille `O(V²)` |
| Build | Makefile | simple, déjà maîtrisé ; CMake possible plus tard |
| Tests | harnais maison | zéro dépendance |
| Gestion d'erreurs | propager (`NULL`/statut) ; `exit()` seulement dans `main` | le C doit pouvoir devenir une lib appelée par un serveur |
| Mesure du temps | `clock_gettime(CLOCK_MONOTONIC)` | mesure de durées fiable |
| Noms de code | identifiants anglais, commentaires/rapport français | convention de l'écosystème C |
| Aléatoire | graine fixe documentée | reproductibilité |

### Conséquence : pas de `exit()` dans les modules

Toute fonction de `src/` (hors `main.c`) doit :
- retourner `NULL` ou un code de statut en cas d'échec ;
- ne jamais appeler `exit()` ;
- laisser `main.c` décider comment réagir (message + code de retour).

## Décisions par phase

### Phase 1 — Dijkstra

| Point | Choix | Pourquoi |
|---|---|---|
| Format de données | CSV `u,v,weight` avec en-tête `n,m` | lisible, généré par Python, diff-able |
| Tas | tas min binaire | `O(E log V)` ; réutilisé par A\* et le VRP |
| Suppression | lazy deletion | simple ; on ignore les entrées périmées au pop |
| Nœuds inatteignables | `INFINITY` | sémantique claire, testable |
| Graphe jouet | exemple CLRS (5 nœuds) | chemins connus → référence |

### Phase 2 — A\*

| Point | Choix | Pourquoi |
|---|---|---|
| Heuristique | Haversine (grand-cercle) | coordonnées lat/lon ; borne inférieure du trajet routier |
| Unité | distance (MVP) | cohérent avec Haversine ; le temps est une extension |
| Arrêt | au **pop** du but | garantit l'optimalité |
| Réouverture | jamais (heuristique consistante) | le premier `g` est définitif |
| Coordonnées | stockées dans `Graph` (optionnel) | A\* en a besoin |

### Phase 3 — Pipeline OSM

| Point | Choix | Pourquoi |
|---|---|---|
| Extraction | OSMnx (Python) | réseau + coordonnées, gère `drive`, évite protobuf/zlib en C |
| Ville | ville moyenne | volume et temps raisonnables |
| Variation de taille | sous-graphes (1k, 5k, 10k, 50k nœuds) | exigé par la consigne |
| Fichiers | `nodes.csv` + `edges.csv` + `meta.txt` | séparation ids / coords / taille |
| Mapping OSM→interne | en Python | le C reste simple |
| Orientation | orienté | sens uniques |
| Poids | distance | cohérent avec l'heuristique |
| Données versionnées | brut ignoré, échantillon + script commités | taille du repo ; reproductible |

### Phase 4 — Matrice de distances

| Point | Choix | Pourquoi |
|---|---|---|
| Algorithme | N Dijkstra | simple et correct |
| Optimisation | arrêt quand toutes les cibles sont atteintes | gain significatif |
| Stockage | dense `double[N][N]` | N modéré, très simple |

### Phase 5 — TSP

| Point | Choix | Pourquoi |
|---|---|---|
| Constructive | plus proche voisin | demandé, `O(N²)` |
| Amélioration | 2-opt | demandé |
| Référence | Held-Karp (≤ ~15 points) | écart à l'optimum |
| Départ | fixe | reproductibilité |

### Phase 6 — CVRP

| Point | Choix | Pourquoi |
|---|---|---|
| Variante | CVRP (capacité, dépôt unique) | correspond à la consigne |
| Demandes | synthétiques, graine fixe | les nœuds OSM n'ont pas de demande |
| Constructive | Clarke & Wright | demandé, `O(N² log N)` |
| Amélioration | Or-opt | demandé |
| Contraintes | capacité seule (MVP) | périmètre minimum |

### Phase 7 — Benchmarks

| Point | Choix | Pourquoi |
|---|---|---|
| Métriques | temps, nœuds settled, qualité | exigé par la consigne |
| Répétitions | ≥ 5, médiane | robuste au bruit |
| Variables | taille graphe × nb points | exigé |
| Référence | Held-Karp (petit N) + meilleur des algos | écart à la meilleure solution |
| Sortie | CSV + plots matplotlib | déjà maîtrisé |
| Environnement | CPU, OS, compilateur, flags, date | reproductibilité |

### Phase 8 — Rapport

| Point | Choix | Pourquoi |
|---|---|---|
| Structure | IMRaD | standard académique |
| Format | Markdown puis PDF si exigé | Obsidian d'abord |
| Articles | section dédiée | exigé par la consigne |
| Complexités | tableau récapitulatif | exigé |
| Compromis | section discussion | exigé |

## Points ouverts (à trancher en groupe)

1. **TSP** : tournée fermée (retour dépôt) ou chemin ouvert ?
2. **CVRP** : objectif = distance minimale, ou flotte minimale puis distance ?
3. **Ville** : laquelle ? (tailles de sous-graphes associées)
4. **Langue du rapport** : français confirmé ?
5. **Format final** : Markdown suffisant ou PDF exigé ?
6. **Bonus** : Contraction Hierarchies ou métaheuristique — après le minimum viable.
