# Plan — Optimisation de tournées logistiques

> [!summary] Statut
> Plan **validé** par l'équipe. Ce document est la source canonique : il décrit les décisions, les phases et les critères de validation. Toute modification de l'architecture doit être discutée en groupe puis reportée ici.

## Contexte

Projet d'implémentation et de comparaison d'algorithmes de graphes appliqués à la logistique : calcul d'itinéraires et planification de tournées de livraison sur un réseau routier réel (OpenStreetMap).

**Fonctionnalités attendues :**

1. Plus court chemin entre deux points (Dijkstra, A\*).
2. Matrice de distances entre points de livraison.
3. Tournée passant par tous les points (TSP).
4. Répartition des livraisons entre plusieurs véhicules à capacité limitée (CVRP).

**Minimum à implémenter :** Dijkstra + A\*, une heuristique constructive (plus proche voisin / Clarke & Wright), une amélioration locale (2-opt / Or-opt).

**Bonus :** Contraction Hierarchies ou métaheuristique (recuit simulé, tabou, génétique).

**Comparaison :** faire varier la taille du graphe et le nombre de points ; mesurer temps de calcul et qualité (distance totale, écart à la meilleure solution connue) ; indiquer les complexités ; discuter du compromis qualité/temps.

**Article :** lire au moins un article de la liste et en présenter les idées principales dans le rapport.

---

## Décisions transverses

| Décision | Options | Choix | Pourquoi |
|---|---|---|---|
| Type des poids | `int` / `float` / `double` | `double` | distances OSM en mètres, futurs coûts « temps » ; évite l'accumulation d'erreurs d'arrondi |
| IDs de nœuds | IDs OSM (épars) / IDs internes denses | IDs internes denses `0..n-1` | `dist[]`/`prev[]` deviennent des tableaux en `O(1)` ; mapping OSM→interne fait en Python |
| Représentation du graphe | matrice `V×V` / liste d'adjacence | liste d'adjacence | graphe routier creux ; la matrice gaspille `O(V²)` |
| Build | Makefile / CMake | Makefile | simple, déjà maîtrisé ; CMake possible plus tard |
| Tests | harnais maison / framework | harnais maison | zéro dépendance ; migration possible si besoin |
| Gestion d'erreurs | `exit()` partout / codes de retour | propager (`NULL`/statut), `exit()` seulement dans `main` | le C doit pouvoir devenir une lib appelée par un serveur |
| Mesure du temps | `clock()` / `gettimeofday()` / `CLOCK_MONOTONIC` | `CLOCK_MONOTONIC` | mesure de durées fiable, insensible à l'horloge système |
| Noms de code | français / anglais | identifiants anglais, commentaires/rapport français | convention de l'écosystème C |
| Aléatoire | non spécifié / graine fixe | graine fixe documentée | reproductibilité académique |
| Branche de travail | `documentation` / `feat/*` / `main` | `main` | base commune unique, moins de conflits |
| Usage de l'IA | libre / sobre | sobre ; doc nettoyée avant rendu | le professeur n'attend pas un projet « fait par IA » |

---

## Phase 0 — Consolider les notes

**Objectif :** partir d'une base propre avant de coder.

**Livrables :**
- `etudes/a_star.md` corrigé (sections 6 et 7).
- `README.md` rempli.
- `doc/test.md` supprimé.

**Décisions :**
- **Notes vs rapport** : `etudes/` (résumés par article) séparé de `report/` (méthodologie/résultats).
- **Commit granulaire** : un commit par unité logique.
- **README** : objectif, arborescence, `make`, état d'avancement.

**Validation :** rien à exécuter ; relecture.

---

## Phase 1 — Dijkstra sur graphe jouet

**Objectif :** poser le socle correct (chargement, tas, plus court chemin) avant les données réelles.

**Livrables :** `src/graph.{h,c}`, `src/heap.{h,c}`, `src/dijkstra.{h,c}`, `src/main.c`, `tests/`, `data/toy/toy1.csv`, `Makefile`.

**Décisions :**

| Point | Options | Choix | Pourquoi |
|---|---|---|---|
| Format de données | CSV `u,v,weight` / binaire / ad hoc | CSV avec en-tête `n,m` | lisible, généré par Python, diff-able |
| Tas | scan linéaire `O(V²)` / tas binaire | tas binaire | attendu pédagogiquement ; réutilisé par A\* et le VRP |
| Suppression | `decrease-key` / lazy deletion | lazy deletion | plus simple ; entrées périmées ignorées au pop |
| Nœuds inatteignables | `-1` / `INFINITY` | `INFINITY` | sémantique claire, testable |
| Départage d'égalités | strict / tolérant | tolérant dans les tests | plusieurs chemins optimaux valides |
| Graphe jouet | arbitraire / exemple CLRS | CLRS (5 nœuds) | chemins connus → référence crédible |

**Stratégie de test :**
- tests unitaires (tas, graphe, dijkstra, reconstruction) ;
- **oracle indépendant** : sur ≤ 8 nœuds, énumérer tous les chemins simples et comparer à Dijkstra.

**Validation :** `make test` vert ; Dijkstra = oracle exhaustif sur les petits graphes.

**Risques :** bugs de tas, reconstruction de chemin.

---

## Phase 2 — A\*

**Objectif :** accélérer Dijkstra sans perdre l'optimalité.

**Livrables :** `src/astar.{h,c}`, extension du graphe (coordonnées), tests, premier benchmark « nœuds settled ».

**Décisions :**

| Point | Options | Choix | Pourquoi |
|---|---|---|---|
| Heuristique | Euclidienne / Manhattan / Haversine | Haversine | nœuds OSM en lat/lon ; distance grand-cercle = borne inférieure du trajet routier |
| Unité des poids | distance / temps | distance (MVP) | Haversine dans la même unité ; le temps exigerait la vitesse max |
| Condition d'arrêt | au pop du but / à son insertion | au pop | seule façon de garantir l'optimalité |
| Réouverture | rouvrir / ne pas rouvrir | ne pas rouvrir | Haversine consistante → le premier `g` est définitif |
| Coordonnées | dans `Graph` / fichier séparé | dans `Graph` (optionnel) | A\* en a besoin ; optionnel pour charger un graphe sans coords |

**Validation croisée :** sur graphe jouet **et** ville, `astar.distance == dijkstra.distance`.

**Métrique clé :** nombre de nœuds *settled* A\* vs Dijkstra.

**Risques :** heuristique non admissible (attrapé par la validation croisée) ; graphe non connexe.

---

## Phase 3 — Pipeline OSM

**Objectif :** passer du jouet à une vraie ville, sans parseur OSM en C.

**Livrables :** `scripts/osm_to_csv.py`, `data/osm/<ville>_*.csv`, `graph_load_csv` opérationnel.

**Décisions :**

| Point | Options | Choix | Pourquoi |
|---|---|---|---|
| Extraction | parseur `.pbf` en C / osmium / OSMnx | OSMnx (Python) | réseau + coordonnées, gère `drive`, simplifie les intersections ; évite protobuf/zlib en C |
| Ville/région | très grande région / ville moyenne | ville moyenne | volume et temps raisonnables |
| Variation de taille | une seule taille / sous-graphes | sous-graphes (ex. 1k, 5k, 10k, 50k nœuds) | la consigne exige de faire varier la taille |
| Format exporté | un fichier / 3 fichiers | `nodes.csv` + `edges.csv` + `meta.txt` | séparation ids / coordonnées / taille |
| Mapping OSM→interne | en C (hash map) / en Python | en Python | le C reste simple (ids denses) |
| Orientation | non orienté / orienté | orienté | sens uniques ; exporter les deux sens si non orienté |
| Poids | distance / temps | distance | cohérent avec l'heuristique |
| Versionnement des données | tout committer / tout ignorer | ignorer le brut, committer un échantillon + le script | éviter de gonfler le repo ; reproductible |
| Reproductibilité | non documenté / documenter | date d'extraction + version OSMnx | les données OSM changent dans le temps |

**Validation :** nombre de nœuds/arêtes identique entre Python et C ; distances plausibles ; garder la plus grande composante connexe.

**Risques :** installation OSMnx, choix de la ville, nœuds isolés, volume de données.

---

## Phase 4 — Matrice de distances

**Objectif :** pré-calculer les distances entre points de livraison (base du TSP/VRP).

**Livrables :** `src/distance_matrix.{h,c}`, export CSV, tests.

**Décisions :**

| Point | Options | Choix | Pourquoi |
|---|---|---|---|
| Algorithme | N Dijkstra / many-to-many avancé | N Dijkstra | simple, correct ; le many-to-many est un bonus hors périmètre |
| Optimisation | Dijkstra complet / arrêt quand toutes les cibles atteintes | arrêt anticipé | gain significatif quand N ≪ V |
| Stockage | dense / creux | dense `double[N][N]` | N ~ 100–1000 → quelques Mo, très simple |
| Ordre de calcul | depuis chaque point / depuis le dépôt | depuis chaque point | matrice complète pour le VRP |

**Validation :** symétrie `d[i][j] == d[j][i]` (graphe non orienté) ; inégalité triangulaire ; cohérence avec des Dijkstra isolés.

**Complexité :** `O(N · (E log V))`.

**Risques :** N grand → temps (mesuré en phase 7).

---

## Phase 5 — TSP (plus proche voisin + 2-opt)

**Objectif :** construire puis améliorer une tournée passant par tous les points.

**Livrables :** `src/tsp.{h,c}` (NN, 2-opt), tests, export de la tournée.

**Décisions :**

| Point | Options | Choix | Pourquoi |
|---|---|---|---|
| Variante | tournée fermée (retour dépôt) / chemin ouvert | à trancher en groupe | impacte les métriques |
| Constructive | plus proche voisin / insertion | NN | demandé, `O(N²)`, simple |
| Amélioration | 2-opt / Or-opt | 2-opt en phase 5 | demandé ; Or-opt en phase 6 |
| Référence qualité | aucune / Held-Karp | Held-Karp ≤ ~15 points | mesure de l'écart à l'optimum |
| Déterminisme | départ aléatoire / fixe | départ fixe | reproductibilité |

**Validation :** chaque point visité exactement une fois ; longueur recalculée = longueur annoncée.

**Complexité :** NN `O(N²)` ; 2-opt `O(N²)` par passe.

**Risques :** minima locaux (documenté via l'écart à l'optimum).

---

## Phase 6 — VRP (capacité limitée)

**Objectif :** répartir les livraisons sur plusieurs véhicules de capacité limitée.

**Livrables :** `src/vrp.{h,c}` (Clarke & Wright + Or-opt), génération de demandes, tests de faisabilité.

**Décisions :**

| Point | Options | Choix | Pourquoi |
|---|---|---|---|
| Variante | VRP ouvert / CVRP | CVRP | correspond à « véhicules de capacité limitée » |
| Demandes | réelles / synthétiques seedées | synthétiques, graine fixe | les points OSM n'ont pas de demande ; reproductibilité |
| Constructive | Clarke & Wright / NN multi-dépôt | Clarke & Wright | demandé ; bonne qualité, `O(N² log N)` |
| Amélioration | Or-opt / 2-opt inter-routes | Or-opt | demandé |
| Objectif | distance seule / flotte puis distance | à trancher en groupe | change la stratégie |
| Contraintes | capacité seule / + fenêtres de temps | capacité seule (MVP) | périmètre minimum |

**Validation :** capacité respectée ; chaque client servi exactement une fois ; routes partent/reviennent au dépôt.

**Complexité :** savings `O(N² log N)` ; Or-opt `O(N²)` par passe.

**Risques :** bugs de contraintes (tests de propriétés).

---

## Phase 7 — Benchmarks

**Objectif :** produire les mesures et courbes demandées (temps vs qualité).

**Livrables :** `benchmarks/bench_*.c`, scripts de lancement, `benchmarks/results/*.csv`, figures.

**Décisions :**

| Point | Options | Choix | Pourquoi |
|---|---|---|---|
| Métriques | temps seul / temps + settled + qualité | les trois | la consigne demande temps et qualité |
| Répétitions | 1 / plusieurs | ≥ 5, médiane | robuste au bruit ; min/max en complément |
| Variables | une dimension / deux | taille graphe × nb points | exigé par la consigne |
| Référence | entre algos / optimum (petit N) | optimum Held-Karp + meilleur des algos | permet l'écart à la meilleure solution |
| Sortie | texte / CSV + graphes | CSV + plots Python/matplotlib | matplotlib déjà maîtrisé |
| Environnement | non documenté / documenter | CPU, OS, compilateur, flags, date | reproductibilité |

**Risques :** bruit de mesure (répétitions, même machine), graphes trop gros.

---

## Phase 8 — Rapport académique

**Objectif :** relier méthodes, résultats et articles.

**Livrables :** `report/01_probleme.md`, `02_methodes.md`, `03_protocole.md`, `04_resultats.md`, `05_references.md`.

**Décisions :**

| Point | Options | Choix | Pourquoi |
|---|---|---|---|
| Langue | français / anglais | français (à confirmer) | cohérent avec notes/consigne |
| Format | Markdown / LaTeX-PDF | Markdown puis export PDF | déjà en Obsidian ; PDF si exigé |
| Structure | libre / IMRaD | IMRaD | standard académique |
| Lien avec les articles | citation vague / section dédiée | section dédiée | exigé par la consigne |
| Complexités | absentes / tableau | tableau récapitulatif | exigé |
| Compromis qualité/temps | implicite / section discussion | section dédiée | exigé |

**Contenu minimum :** formalisation (graphe, TSP, CVRP), description + complexité de chaque algo, protocole, résultats (temps + qualité + écart à l'optimum), discussion, idées principales de l'article choisi.

---

## Points à trancher en groupe

1. **TSP** : tournée fermée (retour dépôt) ou chemin ouvert ?
2. **CVRP** : objectif = distance minimale, ou flotte minimale puis distance ?
3. **Ville** : laquelle ? (tailles des sous-graphes associés)
4. **Langue du rapport** : français confirmé ?
5. **Format final** : Markdown suffisant ou PDF exigé ?
6. **Périmètre bonus** : Contraction Hierarchies ou métaheuristique — après le minimum viable.
