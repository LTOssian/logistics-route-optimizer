# Diapos de la soutenance

Source unique : `soutenance.md` (Marp). Le texte de chaque partie est sur la branche `documentation`, dans `presentation/`. Les copies de `presentation/`, `TASKS.md` et `PLAN.md` présentes sur cette branche ne se modifient pas ici : on les modifie sur `documentation`, puis on fait `git merge documentation` dans `presentation`.

- Les cadres en pointillés sont des emplacements d'images : mettre les PNG dans `img/` et remplacer le cadre par `![](img/nom.png)`.
- Les valeurs en rouge (`À MESURER`) doivent venir de `benchmark## Générer

Depuis la racine du dépôt, avec Docker, sans rien installer d'autre :

```sh
docker run --rm -v "$PWD":/home/marp/app marpteam/marp-cli:v4.1.2 slides/soutenance.md --html --pdf --allow-local-files -o slides/soutenance.pdf
```

`--html` est obligatoire : sans cette option, le schéma, les colonnes et les cadres d'images ne s'affichent pas. Pour une page web, remplacer `--pdf ... -o slides/soutenance.pdf` par `-o slides/soutenance.html`.

Dans Obsidian, le plugin « Marp Slides » affiche un aperçu. Activer l'option HTML pour la même raison.
