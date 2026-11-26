# logistics-route-optimizer

Projet d'optimisation de tournées logistiques : implémentation et comparaison d'algorithmes de graphes (plus court chemin, TSP, VRP) sur un réseau routier réel extrait d'OpenStreetMap.

## Objectif

- **Plus court chemin** : Dijkstra et A\*
- **Matrice de distances** entre points de livraison
- **Tournée (TSP)** : plus proche voisin + 2-opt
- **Tournées de véhicules (CVRP)** : Clarke & Wright + Or-opt
- **Comparaison** temps / qualité, complexités, compromis

## Structure du dépôt

```
.
├── PLAN.md              # plan validé (phases 0 → 9)
├── TASKS.md             # tâches jusqu'à la soutenance
├── presentation/        # base de texte de la soutenance
├── documentation/       # cours (graphes, Dijkstra)
├── etudes/              # notes de lecture des articles
├── report/              # rapport académique (à venir)
├── src/                 # code C : graphe, tas, Dijkstra, rng, main
├── tests/               # tests unitaires, oracle
├── data/                # graphes jouets / export OSM
├── benchmarks/          # protocole et résultats (à venir)
├── scripts/             # pipeline OSM (Python)
├── Dockerfile           # environnement commun
└── compose.yaml
```

## Environnement (Docker)

Tout le monde compile et lance les tests dans la même image : Ubuntu 24.04, gcc 13, valgrind, Python 3.12 avec OSMnx 2.2.0 et matplotlib. Seuls Docker (version 23 ou plus) et Docker Compose v2 sont nécessaires sur la machine. L'image se reconstruit d'elle-même quand le Dockerfile ou les dépendances changent.

```sh
docker compose build                       # une fois, puis à chaque changement du Dockerfile
docker compose run --rm dev make test      # compile + tests
docker compose run --rm dev                # shell dans le conteneur
docker compose run --rm dev valgrind --leak-check=full ./build/test_graph
docker compose run --rm dev python scripts/osm_to_csv.py
```

Le dépôt est monté dans `/work` : on édite sur sa machine, on compile dans le conteneur. Le binaire produit est un exécutable Linux : on le lance toujours via `docker compose run`, y compris pendant la démo.

Pour ajouter une dépendance Python : l'écrire dans `scripts/requirements.txt`, puis régénérer le fichier de versions avec `docker compose run --rm dev pip freeze > scripts/requirements.lock`.

Sous Linux, pour que `build/` appartienne à son utilisateur : `export HOST_UID=$(id -u) HOST_GID=$(id -g)` avant `docker compose run`.

Sur un réseau d'entreprise qui inspecte le TLS (erreur `CERTIFICATE_VERIFY_FAILED` pendant le build), indiquer le certificat racine du réseau avant les commandes :

```sh
export EXTRA_CA_FILE=/chemin/vers/ca.pem
```

La CI GitHub (`.github/workflows/ci.yml`) lance `make test` et valgrind dans cette même image à chaque push sur `main` et sur chaque PR.

## Build

```sh
make        # compile lro et les tests
make test   # compile + exécute les tests
make clean  # nettoie les artefacts
```

## Utilisation

```sh
docker compose run --rm dev ./build/lro path data/toy/toy1.csv 0 4
docker compose run --rm dev ./build/lro path data/toy/toy1.csv 0 4 --astar data/toy/toy1_nodes.csv
```

## Données de Lyon

Les graphes sont déjà dans `data/osm/lyon_<R>/` : carrés de demi-côté R = 500, 1000, 2000 et 4000 m autour de Bellecour, réseau « drive ». Le graphe est non orienté : les sens uniques sont ignorés. Chaque dossier contient `edges.csv`, `nodes.csv` et `meta.txt` (paramètres, tailles, corrections appliquées).

Ces données sont **gelées** : tout le monde travaille sur les mêmes nœuds. Sans le cache local, Overpass renvoie la carte OSM du jour, donc une régénération peut donner d'autres nœuds et une autre numérotation. Le script refuse d'écraser un dossier existant sans `--force`.

```sh
docker compose run --rm dev python scripts/osm_to_csv.py --out /tmp/osm               # essai, sans toucher aux données
docker compose run --rm dev python scripts/osm_to_csv.py --radius 500 --force         # écrase lyon_500
docker compose run --rm dev python scripts/osm_to_csv.py --overpass-url <miroir>/api  # si overpass-api.de sature
```

| R | Nœuds | Rues | Arêtes orientées |
|---|---|---|---|
| 500 m | 154 | 224 | 448 |
| 1000 m | 550 | 850 | 1 700 |
| 2000 m | 1 886 | 3 021 | 6 042 |
| 4000 m | 5 344 | 8 128 | 16 256 |

## État d'avancement

- [x] Notes de lecture : Dijkstra, A\*, Contraction Hierarchies
- [x] Décisions techniques et plan validés
- [x] Phase 1 — Dijkstra sur graphe jouet
- [x] Phase 2 — A\*
- [x] Phase 3 — pipeline OSM
- [x] Phase 4 — matrice de distances
- [ ] Phase 5 — TSP
- [ ] Phase 6 — CVRP
- [ ] Phase 7 — benchmarks
- [ ] Phase 8 — rapport
- [ ] Phase 9 — soutenance (vendredi 4 décembre 2026)

## Documents

- Plan détaillé : [`PLAN.md`](PLAN.md)
- Notes de lecture : [`etudes/`](etudes/)
- Tâches : [`TASKS.md`](TASKS.md), issues GitHub du milestone « Soutenance »
- Soutenance : [`presentation/`](presentation/) (texte), branche `presentation` (diapos)

## Usage de l'IA

On a utilisé un assistant IA (Claude) comme outil d'appui : pour structurer le plan, mettre en place l'environnement Docker, relire le code et préparer une base de texte pour la soutenance. Le contexte qu'on lui donne est dans `PROMPT/`. Le code et les textes finaux sont relus, compris et modifiés par le groupe, et chacun peut expliquer ce qu'il présente.

## Branches

- `main` : code testé, une PR par issue, CI obligatoire. C'est la branche rendue : `documentation` y est mergée avant le rendu.
- `documentation` : ce vault Obsidian (plan, tâches, notes, soutenance, rapport).
- `presentation` : diapos Marp.
