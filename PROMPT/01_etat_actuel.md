# 01 — État actuel du dépôt

Mis à jour le 26 novembre 2026. En cas de doute, vérifier `git log` et `TASKS.md`.

## Branches

| Branche | Contenu |
|---|---|
| `main` | plan, notes de lecture, Docker et CI (PR #2), phase 1 complète (PR #15 et #16) |
| `documentation` | vault Obsidian : plan, tâches, base de la soutenance |
| `presentation` | diapos Marp |

## Ce qui existe

- Notes de lecture (`etudes/`) : Dijkstra, A\*, Contraction Hierarchies.
- Phase 1 mergée dans `main` : graphe, tas, Dijkstra (naive, tas, query), générateur `rng`, programme `lro path`, tests dont l'oracle exhaustif. `make test` est vert et valgrind ne signale aucune fuite, vérifiés par la CI. Visite du code : `presentation/code.md`.
- Image Docker commune et CI, mergées dans `main`.
- Base de texte de la soutenance (`presentation/`) et liste des tâches (`TASKS.md`).

## Ce qui manque

- Retouches de la phase 1 relevées en review : #17.
- Phases 2 à 8 : rien de codé.
- Note de lecture Clarke & Wright.

## Prochaine étape

Voir `../TASKS.md` et le calendrier de la phase 9 dans `../PLAN.md`. Soutenance le vendredi 4 décembre 2026.
