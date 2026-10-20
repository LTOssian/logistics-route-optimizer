# 01 — État actuel du dépôt

> [!info] Dernière mise à jour
> Ce fichier décrit l'état au moment de la rédaction du plan. Vérifier `git log` en cas de doute.

## Branches

- `main` : **branche de travail unique** (code, docs, livrables).
- `documentation` : (optionnel) notes de lecture.

> [!important] On travaille directement sur `main`. Voir `05_workflow_git.md`.

## Ce qui existe

### Notes de lecture (`etudes/`)

| Fichier | Contenu | État |
|---|---|---|
| `djikstra.md` | Résumé de l'article de Dijkstra (MST + plus court chemin, ensembles A/B/C) | complet |
| `a_star.md` | A\* : problématique, `f=g+h`, admissible/consistante, pseudo-code, structures C, complexité, cas limites, réouverture | complet |
| `Contraction Hierarchies.md` | CH : prétraitement, witness search, ordre des nœuds, requête bidirectionnelle, stall-on-demand | complet |

### Documentation projet

| Fichier | Contenu |
|---|---|
| `README.md` | Objectif, arborescence cible, build, état d'avancement |
| `PLAN.md` | Plan validé, phases 0 → 8 (source canonique) |
| `PROMPT/` | Ce dossier de contexte |

### Configuration

- `.gitignore` : ignore `.obsidian/*`, `.trash/`, `.DS_Store`, `*.tmp`.
- `doc/test.md` : supprimé (placeholder).

## Ce qui manque encore

- `src/` : aucun code C.
- `tests/` : aucune suite de tests.
- `Makefile` : absent.
- `data/` : aucun graphe (jouet ou OSM).
- `scripts/` : aucun pipeline OSM.
- `benchmarks/` : aucun protocole ni résultat.
- `report/` : aucun rapport.

## Prochaine étape

**Phase 1** (voir `03_plan.md`) : implémenter Dijkstra sur un graphe jouet, avec tas min et tests unitaires.

Ordre recommandé :
1. Créer le squelette (`Makefile`, `src/`, `tests/`, `data/toy/`).
2. Écrire `graph.{h,c}` et `heap.{h,c}` + tests.
3. Écrire `dijkstra.{h,c}` + tests (oracle exhaustif sur petits graphes).
4. `main.c` de démonstration.
5. Commit.

## Conventions de branches

Le travail se fait **directement sur `main`**. `documentation` n'est qu'une branche de notes optionnelle. Voir `05_workflow_git.md`.
