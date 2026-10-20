# PROMPT — Contexte pour les agents IA

> [!summary] À quoi sert ce dossier
> Il contient tout le contexte nécessaire pour qu'un agent IA (ou un nouveau membre) travaille sur le projet **sans repartir de zéro**. Lis les fichiers dans l'ordre.

## Ordre de lecture

1. `00_contexte_projet.md` — la consigne et les objectifs.
2. `01_etat_actuel.md` — où en est le dépôt aujourd'hui.
3. `02_decisions.md` — les décisions techniques validées (et les points ouverts).
4. `03_plan.md` — le plan par phases (copie de `../PLAN.md`).
5. `04_conventions.md` — style C, mémoire, erreurs, build, tests.
6. `05_workflow_git.md` — branches, commits, push.
7. `06_references.md` — articles et notes de lecture.
8. `07_agent_instructions.md` — le prompt système à donner à l'agent.
9. `08_glossaire.md` — définitions des termes du projet.

## Règle d'or

- Le code est en **C** (C11). Les algorithmes vont dans `src/`, les tests dans `tests/`, les données dans `data/`.
- Ne pas modifier l'architecture ni les décisions de `02_decisions.md` sans validation du groupe.
- Toujours compiler avec `-Wall -Wextra -Werror` et lancer les tests avant de committer.

## Arborescence cible

```
.
├── PLAN.md
├── PROMPT/
├── etudes/
├── report/
├── src/
├── tests/
├── data/
├── benchmarks/
├── scripts/
└── Makefile
```
