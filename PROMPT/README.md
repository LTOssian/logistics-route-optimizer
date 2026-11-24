# PROMPT — Contexte pour les agents IA

> [!summary] À quoi sert ce dossier
> Il contient tout le contexte nécessaire pour qu'un agent IA (ou un nouveau membre) travaille sur le projet **sans repartir de zéro**. Lis les fichiers dans l'ordre.

## Ordre de lecture

1. `00_contexte_projet.md` — la consigne et les objectifs.
2. `01_etat_actuel.md` — où en est le dépôt aujourd'hui.
3. `../PLAN.md` — décisions validées et plan par phases (source unique).
4. `04_conventions.md` — style C, mémoire, erreurs, build, tests.
5. `05_workflow_git.md` — branches, commits, PR, Docker.
6. `06_references.md` — articles et notes de lecture.
7. `07_agent_instructions.md` — le prompt système à donner à l'agent.
8. `08_glossaire.md` — définitions des termes du projet.

## Règle d'or

- Le code est en **C** (C11). Les algorithmes vont dans `src/`, les tests dans `tests/`, les données dans `data/`.
- Ne pas modifier l'architecture ni les décisions de `../PLAN.md` sans validation du groupe.
- Toujours compiler avec `-Wall -Wextra -Werror` et lancer les tests (dans Docker) avant de committer.

## Arborescence cible

```
.
├── PLAN.md
├── TASKS.md
├── PROMPT/
├── presentation/
├── etudes/
├── report/
├── src/
├── tests/
├── data/
├── benchmarks/
├── scripts/
├── Dockerfile
├── compose.yaml
└── Makefile
```
