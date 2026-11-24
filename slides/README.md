# Diapos de la soutenance

Source unique : `soutenance.md` (Marp). Le texte de chaque partie est sur la branche `documentation`, dans `presentation/`.

- Les cadres en pointillés sont des emplacements d'images : mettre les PNG dans `img/` et remplacer le cadre par `![](img/nom.png)`.
- Les valeurs en rouge (`À MESURER`) doivent venir de `benchmarks/results/`. Il ne doit plus en rester le 2 décembre.
- Le nom de l'orateur est en haut à droite de chaque diapo. Les notes orateur sont dans les commentaires `<!-- -->`.

## Générer

Avec Docker, sans rien installer d'autre :

```sh
docker run --rm -v "$PWD":/home/marp/app marpteam/marp-cli:v4.1.2 slides/soutenance.md --html --pdf --allow-local-files -o slides/soutenance.pdf
```

Remplacer `--pdf` par `--html` pour une page web, ou ajouter `--watch` pour voir les modifications en direct.

Dans Obsidian, le plugin « Marp Slides » affiche un aperçu. Activer l'option HTML pour que le schéma du pipeline s'affiche.
