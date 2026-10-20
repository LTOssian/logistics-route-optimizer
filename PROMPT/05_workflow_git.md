# 05 — Workflow Git

## Modèle de branches

| Branche | Rôle | Qui y touche |
|---|---|---|
| `main` | **stable** : code + README | merges validés uniquement |
| `documentation` | notes de lecture, `PLAN.md`, `PROMPT/`, rapport | rédaction |
| `feat/<sujet>` | travail en cours (code) | une branche par phase/tâche |

- Les branches `feat/*` partent de `main` (ou de `documentation` si c'est de la doc).
- Une branche de feature est **courte** : on la fusionne dès que les tests passent.
- Fusionner `documentation` → `main` aux jalons (fin de phase) pour que le code et les docs vivent ensemble.

## Commits

Format **Conventional Commits** :

```
<type>: <sujet court à l'impératif>
```

Types : `feat`, `fix`, `docs`, `test`, `chore`, `refactor`, `bench`, `perf`.

Exemples :
```
feat: graph + min-heap modules
test: dijkstra against exhaustive oracle
fix: heap pop order on equal keys
docs: phase 1 notes
chore: add Makefile and .gitignore entries
```

Règles :
- **Un commit = une chose** (pas un commit fourre-tout en fin de TP).
- Sujet ≤ ~50 caractères ; corps si nécessaire pour expliquer le *pourquoi*.
- Ne jamais committer d'artefacts (`*.o`, binaires, `build/`, `.obsidian/*`).

## Push et synchronisation

- `git pull --rebase` avant de commencer à travailler (évite les merges de bruit).
- Pousser régulièrement sur sa branche.
- Ne **pas** force-push `main` ni `documentation` sans coordination.
- Sur sa **propre** branche, si rebase nécessaire : `git push --force-with-lease`.

## Extension Obsidian Git

Le plugin **Obsidian Git** peut auto-committer/pousser périodiquement. Pour éviter les conflits :

1. **Pull avant de travailler** (le plugin peut avoir poussé).
2. **Ne pas éditer les mêmes fichiers que le plugin en même temps** (surtout `workspace.json`, ignoré).
3. Si le plugin crée un commit automatique, ce n'est pas grave : ne pas réécrire l'historique.
4. Pour le code, préférer le terminal / l'éditeur, pas Obsidian.

## Revue

Pour une petite équipe, une revue légère suffit. Checklist avant de merger dans `main` :

- [ ] `make` compile sans warning (`-Werror`).
- [ ] `make test` vert.
- [ ] Pas de fuite mémoire (valgrind si disponible).
- [ ] Commit(s) conventionnel(s) et lisibles.
- [ ] Documentation / commentaires à jour.
- [ ] Pas d'artefact committé.

## Definition of Done (phase)

Une phase est terminée quand :

1. Les livrables listés dans `03_plan.md` existent.
2. Les critères de validation de la phase passent.
3. Le README/état d'avancement est mis à jour.
4. Le rapport reçoit la section correspondante (au fur et à mesure, pas à la fin).
5. La branche est fusionnée dans `main` et poussée.

## Résolution de conflits (rappel)

- Conflit sur un fichier de doc → discuter avec l'auteur, fusionner à la main.
- Conflit sur `workspace.json` → il est ignoré, il ne devrait pas arriver.
- Jamais de `git commit -a` sans avoir relu `git diff --staged`.
