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
└── scripts/             # pipeline OSM (Python)
```

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
