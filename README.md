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
├── etudes/              # notes de lecture des articles
├── report/              # rapport académique (à venir)
├── src/                 # code C (à venir)
├── tests/               # tests unitaires (à venir)
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
make        # compile
make test   # compile + exécute les tests
make clean  # nettoie les artefacts
```

## État d'avancement

- [x] Notes de lecture : Dijkstra, A\*, Contraction Hierarchies
- [x] Décisions techniques et plan validés
- [ ] Phase 1 — Dijkstra sur graphe jouet (V1 faite, tas en cours)
- [ ] Phase 2 — A\*
- [ ] Phase 3 — pipeline OSM
- [ ] Phase 4 — matrice de distances
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
