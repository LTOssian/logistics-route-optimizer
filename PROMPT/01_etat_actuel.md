# 01 — État actuel du dépôt

Mis à jour le 24 novembre 2026. En cas de doute, vérifier `git log` et `TASKS.md`.

## Branches

| Branche | Contenu |
|---|---|
| `main` | plan, notes de lecture, Docker et CI (PR #2), phase 1 V1 (PR #15) |
| `1-phase-1-dijkstra-sur-graphe-jouet` | suite de la phase 1 (tas, CLI, oracle), voir #1 |
| `documentation` | vault Obsidian : plan, tâches, base de la soutenance |
| `presentation` | diapos Marp |

## Ce qui existe

- Notes de lecture (`etudes/`) : Dijkstra, A\*, Contraction Hierarchies.
- Phase 1, V1 mergée dans `main` : `src/graph.{h,c}`, `src/djikstra.{h,c}`, tests du graphe et de Dijkstra, graphe jouet `data/toy/toy1.csv` et ses résultats attendus. `make test` est vert et valgrind ne signale aucune fuite, vérifiés par la CI.
- Image Docker commune et CI, mergées dans `main`.
- Base de texte de la soutenance (`presentation/`) et liste des tâches (`TASKS.md`).

## Ce qui manque

- Phase 1 : tas binaire, `main.c` (CLI `lro`), oracle exhaustif, renommage `djikstra` → `dijkstra`.
- Phases 2 à 8 : rien de codé.
- Note de lecture Clarke & Wright.

## Prochaine étape

Voir `../TASKS.md` et le calendrier de la phase 9 dans `../PLAN.md`. Soutenance le vendredi 4 décembre 2026.
