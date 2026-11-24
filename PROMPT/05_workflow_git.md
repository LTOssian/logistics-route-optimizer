# 05 — Workflow Git

## Modèle de branches

| Branche | Rôle | Comment on y écrit |
|---|---|---|
| `main` | code testé : `src/`, `tests/`, `scripts/`, `benchmarks/`, Docker | uniquement par PR, CI verte |
| `<n°>-<titre>` | une branche par issue (bouton « Create a branch » de l'issue) | push libre par la personne assignée |
| `documentation` | vault Obsidian : `PLAN.md`, `TASKS.md`, `etudes/`, `presentation/`, `report/` | push direct |
| `presentation` | diapos Marp (`slides/`) | push direct |

- Une issue = une branche courte, mergée dès que la CI passe.
- `documentation` suit `main` : on y merge `main` de temps en temps (`git merge main`), sans rebase.
- `presentation` suit `documentation` de la même façon.

## Commits

Format **Conventional Commits** :

```
<type>: <sujet court à l'impératif>
```

Types : `feat`, `fix`, `docs`, `test`, `chore`, `refactor`, `bench`, `perf`.

Exemples :
```
feat: ajoute le tas binaire
test: compare dijkstra à l'oracle exhaustif
fix: ordre du pop à clés égales
docs: notes de la phase 1
```

Sujets en français, courts.

Règles :
- **Un commit = une chose** (pas un commit fourre-tout en fin de TP).
- Sujet ≤ ~50 caractères ; corps si nécessaire pour expliquer le *pourquoi*.
- Ne jamais committer d'artefacts (`*.o`, binaires, `build/`, `.obsidian/*`).

## Push et synchronisation

- `git pull --rebase` avant de commencer à travailler (évite les merges de bruit).
- Pousser régulièrement sa branche ; ouvrir la PR tôt (en brouillon si besoin).
- Ne **jamais** force-push `main`, `documentation` ni `presentation`.
- Sur sa **propre** branche, si rebase nécessaire : `git push --force-with-lease`.

## Extension Obsidian Git

Le plugin **Obsidian Git** peut auto-committer/pousser périodiquement. Pour éviter les conflits :

1. **Pull avant de travailler** (le plugin peut avoir poussé).
2. **Ne pas éditer les mêmes fichiers que le plugin en même temps** (surtout `workspace.json`, ignoré).
3. Si le plugin crée un commit automatique, ce n'est pas grave : ne pas réécrire l'historique.
4. Pour le code, préférer le terminal / l'éditeur, pas Obsidian.

## Revue

Pour une petite équipe, une revue légère suffit : un des deux autres relit la PR. La CI (`.github/workflows/ci.yml`) lance `make test` et valgrind dans l'image Docker. Checklist avant de demander la revue :

- [ ] `docker compose run --rm dev make test` vert, sans warning (`-Werror`).
- [ ] Pas de fuite mémoire (valgrind, lancé aussi par la CI).
- [ ] Commit(s) conventionnel(s) et lisibles.
- [ ] Documentation / commentaires à jour.
- [ ] Pas d'artefact committé.

## Definition of Done (phase)

Une phase est terminée quand :

1. Les livrables listés dans `../PLAN.md` existent.
2. Les critères de validation de la phase passent.
3. Le README/état d'avancement est mis à jour.
4. Le rapport reçoit la section correspondante (au fur et à mesure, pas à la fin).
5. La PR est mergée dans `main`.

## Résolution de conflits (rappel)

- Conflit sur un fichier de doc → discuter avec l'auteur, fusionner à la main.
- Conflit sur `workspace.json` → il est ignoré, il ne devrait pas arriver.
- Jamais de `git commit -a` sans avoir relu `git diff --staged`.
