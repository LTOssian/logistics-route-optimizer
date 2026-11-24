# Tâches — soutenance du vendredi 4 décembre

On coche ici au fil de l'eau, et on ferme l'issue GitHub correspondante quand la PR est mergée. Le détail de chaque tâche est dans l'issue, les décisions dans [[PLAN]].

Légende : **L** = Louisan, **S** = Sofia, **F** = Faustine.

## Mar. 24 – mer. 25 nov.

- [ ] Relire et merger la PR #2 (Docker + CI) — S ou F
- [ ] Chacun : `docker compose build` puis `docker compose run --rm dev make test` sur la branche de la phase 1 — L, S, F
- [ ] Phase 1 : renommage `dijkstra`, tas binaire, Dijkstra V2, oracle, CLI `path` (#1) — L
- [ ] Phase 3 : `scripts/osm_to_csv.py`, graphes Lyon 500 à 4000 m (#4) — S
- [ ] Phase 5 : NN + 2-opt + Held-Karp sur matrices jouets (#6) — F

## Jeu. 26 – ven. 27 nov.

- [ ] Phase 2 : A\* + Haversine (#3) — L
- [ ] Phase 4 : matrice de distances (#5) — L
- [ ] Phase 6 : Clarke & Wright + Or-opt sur matrices jouets (#7) — F
- [ ] Note de lecture Clarke & Wright (#10) — F
- [ ] Phase 7 : script de benchmarks et protocole, `benchmarks/ENV.md` (#8) — S
- [ ] Lecteurs TSPLIB / CVRPLIB, optimums relus sur les sites officiels (#6, #7) — F

## Sam. 28 – lun. 30 nov.

- [ ] TSP et CVRP branchés sur la matrice de Lyon, sous-commandes `tsp` et `cvrp` (#6, #7) — F
- [ ] Benchmarks B1 à B4 lancés, CSV dans `benchmarks/results/` (#8) — S
- [ ] Figures `benchmarks/plot.py` (#8) — S
- [ ] `scripts/plot_route.py` et cartes (#12) — S
- [ ] **Lundi soir : gel du code.** Après, seulement des corrections de bugs — L, S, F

## Mar. 1 – mer. 2 déc.

- [ ] Diapos remplies avec les vrais chiffres, plus aucun `À MESURER` (#13) — L, S, F
- [ ] `demo.sh` + vidéo de secours + test sur une machine propre (#11) — L
- [ ] Rapport : chacun sa section (#9) — L, S, F
- [ ] Export PDF des diapos (#13) — L

## Jeu. 3 déc.

- [ ] Répétition 1 chronométrée (#14) — L, S, F
- [ ] Ajustements
- [ ] Répétition 2 avec la démo (#14) — L, S, F
- [ ] Révision des [[presentation/questions_jury|questions du prof]] — L, S, F

## Ven. 4 déc. — avant de partir

- [ ] Poste de démo : image Docker construite, `data/osm/lyon_2000` présent, `make` fait
- [ ] PDF des diapos + vidéo de secours sur clé USB
- [ ] Repo propre : brouillons et `PROMPT/` retirés de la version rendue si non demandés

## Hors calendrier (seulement si tout le reste est fini)

- [ ] Bonus : Contraction Hierarchies ou recuit simulé
