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
├── PLAN.md              # plan validé (phases 0 → 8)
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

Tout le monde compile et lance les tests dans la même image : Ubuntu 24.04, gcc 13, valgrind, Python 3.12 avec OSMnx 2.2.0 et matplotlib. Seuls Docker et Docker Compose sont nécessaires sur la machine.

```sh
docker compose build                       # une fois, puis à chaque changement du Dockerfile
docker compose run --rm dev make test      # compile + tests
docker compose run --rm dev                # shell dans le conteneur
docker compose run --rm dev valgrind --leak-check=full ./build/test_graph
docker compose run --rm dev python scripts/osm_to_csv.py
```

Le dépôt est monté dans `/work` : on édite sur sa machine, on compile dans le conteneur.

Sous Linux, pour que `build/` appartienne à son utilisateur : `export HOST_UID=$(id -u) HOST_GID=$(id -g)` avant `docker compose run`.

Sur un réseau d'entreprise qui inspecte le TLS (erreur `CERTIFICATE_VERIFY_FAILED` pendant le build), construire l'image en fournissant le certificat racine du réseau :

```sh
docker build --secret id=extra_ca,src=mon_ca.pem -t lro-dev:1 .
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
- [ ] Phase 1 — Dijkstra sur graphe jouet
- [ ] Phase 2 — A\*
- [ ] Phase 3 — pipeline OSM
- [ ] Phase 4 — matrice de distances
- [ ] Phase 5 — TSP
- [ ] Phase 6 — CVRP
- [ ] Phase 7 — benchmarks
- [ ] Phase 8 — rapport

## Documents

- Plan détaillé : [`PLAN.md`](PLAN.md)
- Notes de lecture : [`etudes/`](etudes/)
