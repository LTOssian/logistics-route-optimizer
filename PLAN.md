# Plan — Optimisation de tournées logistiques

> [!summary] Statut
> Plan **validé** par l'équipe. Ce document est la source canonique : il décrit les décisions, les phases et les critères de validation. Toute modification de l'architecture doit être discutée en groupe puis reportée ici.
>
> Soutenance : **vendredi 4 décembre 2026**, 15 minutes, démo live. Calendrier et répartition en phase 9. Suivi au jour le jour dans `TASKS.md`.

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
| Branches | tout sur `main` / une branche par issue | une branche par issue, PR vers `main` ; `documentation` pour la doc Obsidian ; `presentation` pour les diapos | à trois en asynchrone, la CI vérifie chaque PR avant le merge |
| Environnement | installations locales / Docker | image Docker commune (`compose.yaml`) | mêmes versions de gcc, valgrind et OSMnx pour tout le monde, et pour la démo |
| Usage de l'IA | libre / sobre | sobre ; doc nettoyée avant rendu | le professeur n'attend pas un projet « fait par IA » |

---

## Phase 0 — Consolider les notes

**Statut :** terminée.

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
| Tas | scan linéaire `O(V²)` / tas binaire | les deux : V1 en scan linéaire (faite), V2 avec tas binaire | la V1 est simple à vérifier ; la V2 est nécessaire sur OSM et réutilisée par A\* ; comparer les deux donne une courbe pour la soutenance |
| Nommage | `djikstra` / `dijkstra` | `dijkstra` (renommer fichiers et tests) | orthographe correcte, visible pendant la démo |
| CLI | aucune / `lro` | binaire `build/lro` avec sous-commandes `path`, `tsp`, `cvrp` | sert à la démo live |
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
| Ville/région | très grande région / ville moyenne | Lyon, centrée place Bellecour | données OSM complètes, téléchargeables directement avec OSMnx |
| Variation de taille | une seule taille / sous-graphes | disques de rayon 500, 1000, 2000 et 4000 m autour du centre (`graph_from_point`) | la consigne exige de faire varier la taille ; un seul paramètre à expliquer |
| Où lancer le script | machine de chacun / Docker | Docker (`docker compose run --rm dev python scripts/osm_to_csv.py`) | même version d'OSMnx pour tous |
| Format exporté | un fichier / 3 fichiers | `nodes.csv` + `edges.csv` + `meta.txt` | séparation ids / coordonnées / taille |
| Mapping OSM→interne | en C (hash map) / en Python | en Python | le C reste simple (ids denses) |
| Orientation | non orienté / orienté | non orienté (chaque rue exportée dans les deux sens) | matrice symétrique, donc 2-opt classique ; les sens uniques sont cités dans les limites |
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
| Variante | tournée fermée (retour dépôt) / chemin ouvert | tournée fermée | un livreur revient à l'entrepôt ; cohérent avec le CVRP |
| Points | choisis à la main / tirés au hasard | dépôt = nœud le plus proche du centre ; clients tirés parmi les nœuds, graine 42 | reproductible |
| Constructive | plus proche voisin / insertion | NN | demandé, `O(N²)`, simple |
| Amélioration | 2-opt / Or-opt | 2-opt en phase 5 | demandé ; Or-opt en phase 6 |
| Référence qualité | aucune / Held-Karp | Held-Karp ≤ ~12 points, plus l'instance TSPLIB `berlin52` (optimum publié) | mesure de l'écart à l'optimum, y compris sur une instance plus grande |
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
| Demandes | réelles / synthétiques seedées | entiers de 1 à 10, graine 42 ; capacité Q = 60 par défaut | les points OSM n'ont pas de demande ; reproductibilité |
| Constructive | Clarke & Wright / NN multi-dépôt | Clarke & Wright | demandé ; bonne qualité, `O(N² log N)` |
| Amélioration | Or-opt / 2-opt inter-routes | Or-opt | demandé |
| Objectif | distance seule / flotte puis distance | distance totale ; le nombre de véhicules est un résultat | plus simple, et c'est ce que Clarke & Wright optimise naturellement |
| Référence qualité | aucune / instance publique | instance CVRPLIB `A-n32-k5` (optimum publié) | seule façon simple de mesurer un écart à l'optimum en CVRP |
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
| Langue | français / anglais | français | cohérent avec notes/consigne |
| Format | Markdown / LaTeX-PDF | Markdown puis export PDF | déjà en Obsidian ; PDF si exigé |
| Structure | libre / IMRaD | IMRaD | standard académique |
| Lien avec les articles | citation vague / section dédiée | section dédiée | exigé par la consigne |
| Complexités | absentes / tableau | tableau récapitulatif | exigé |
| Compromis qualité/temps | implicite / section discussion | section dédiée | exigé |

**Contenu minimum :** formalisation (graphe, TSP, CVRP), description + complexité de chaque algo, protocole, résultats (temps + qualité + écart à l'optimum), discussion, idées principales de l'article choisi.

---

## Phase 9 — Soutenance

**Objectif :** présenter le projet en 15 minutes, démo live comprise, le vendredi 4 décembre 2026.

**Livrables :** `presentation/` (texte de chaque partie, sur la branche `documentation`), diapos Marp (branche `presentation`), script de démo et vidéo de secours.

**Décisions :**

| Point | Options | Choix | Pourquoi |
|---|---|---|---|
| Outil des diapos | PowerPoint / Google Slides / Marp | Marp (Markdown) | versionné dans git, éditable par les trois, export PDF/HTML |
| Démo | captures / live | live dans le conteneur Docker, avec une vidéo en secours | montre que l'outil fonctionne ; la vidéo protège d'un plantage |
| Répartition | une personne / chacun sa partie | chacun présente ce qu'il a codé | chacun peut défendre son code |
| Chiffres | estimés / mesurés | uniquement ceux de `benchmarks/results/` | pas de résultat inventé |

**Répartition :** pas encore faite. Chacun s'assigne les issues qu'il prend sur GitHub, et présente ensuite les parties qu'il a codées.

Le rapport (phase 8) est écrit par tous : chacun rédige la section de ses phases.

**Calendrier :**

| Dates | Objectif |
|---|---|
| mar. 24 – mer. 25 nov. | Docker mergé ; phase 1 terminée (tas, CLI, renommage) ; pipeline OSM ; TSP sur des matrices jouets |
| jeu. 26 – ven. 27 nov. | A\* ; matrice ; CVRP sur des matrices jouets ; script de benchmarks |
| sam. 28 – lun. 30 nov. | tout branché sur Lyon ; benchmarks et figures ; **gel du code lundi soir** |
| mar. 1 – mer. 2 déc. | diapos avec les vrais chiffres ; rapport ; vidéo de secours |
| jeu. 3 déc. | deux répétitions chronométrées |
| ven. 4 déc. | soutenance |

Le TSP et le CVRP ne dépendent que d'une matrice de distances : ils peuvent être codés et testés sur des matrices jouets (ou TSPLIB / CVRPLIB) avant que la phase 4 soit prête.

**Validation :** répétition complète en moins de 15 minutes ; démo jouée de bout en bout sur une machine propre (`docker compose build` puis `demo.sh`).

---

## Décisions tranchées le 24 novembre

1. TSP : tournée fermée.
2. CVRP : minimiser la distance totale, flotte libre.
3. Ville : Lyon, disques de 500 à 4000 m autour de Bellecour.
4. Graphe non orienté, pour une matrice symétrique.
5. Rapport en français, en Markdown (export PDF si demandé).
6. Bonus (Contraction Hierarchies ou métaheuristique) : seulement si tout le minimum est fini avant le gel du code.
