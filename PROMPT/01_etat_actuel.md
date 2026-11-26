# 01 — État actuel du dépôt

Mis à jour le 26 novembre 2026. En cas de doute, vérifier `git log` et `TASKS.md`.

## Branches

| Branche | Contenu |
|---|---|
| `main` | plan, notes de lecture, Docker et CI (PR #2), phase 1 complète (PR #15 et #16) |
| `3-phase-2-a-avec-heuristique-haversine` | phase 2, A\* : code et tests faits, PR à ouvrir (reste `--astar` dans `lro path`) |
| `4-phase-3-pipeline-osm-lyon` | phase 3, données de Lyon : PR #18 à relire et merger |
| `documentation` | vault Obsidian : plan, tâches, base de la soutenance |
| `presentation` | diapos Marp |

## Ce qui existe

- Notes de lecture (`etudes/`) : Dijkstra, A\*, Contraction Hierarchies.
- Phase 1 mergée dans `main` : graphe, tas, Dijkstra (naive, tas, query), générateur `rng`, programme `lro path`, tests dont l'oracle exhaustif. `make test` est vert et valgrind ne signale aucune fuite, vérifiés par la CI. Visite du code : `presentation/code.md`.
- Image Docker commune et CI, mergées dans `main`.
- Base de texte de la soutenance (`presentation/`) et liste des tâches (`TASKS.md`).

## Ce qui manque

- Retouches de la phase 1 relevées en review : #17.
- Phases 4 à 8 : rien de codé.
- Note de lecture Clarke & Wright.

## Ordre de merge

1. Phase 3 (#18) : ne touche aucun fichier C, met les données de Lyon sur `main`.
2. Phase 2 (#3) : après avoir mergé `main` dans sa branche, ajoute le test A\* contre Dijkstra sur Lyon. Inclure le point 1 de #17 (lecture des arguments de `lro path`), qui touche le même code.
3. Phase 4 (#5) : part de `main` après la phase 2, qui modifie aussi `main.c` et le `Makefile`.

## Prochaine étape

Voir `../TASKS.md` et le calendrier de la phase 9 dans `../PLAN.md`. Soutenance le vendredi 4 décembre 2026.
