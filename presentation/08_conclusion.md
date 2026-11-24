# 8. Conclusion et limites

Orateur : à définir — 1 min 30 · Vocabulaire : [[glossaire]]

## Message clé

On a une chaîne complète, testée et mesurée. Ses limites sont connues et on sait ce qu'on ferait ensuite.

## Diapos

**Diapo 14 — Bilan : le compromis qualité / temps**

- Ce qui marche : plus court chemin exact (Dijkstra, A\*), matrice, TSP, CVRP sur une vraie ville.
- Figure : qualité (écart à l'optimum) en fonction du temps, pour NN, NN + 2-opt, C&W, C&W + Or-opt.
- Ce qu'on a appris : la structure de données change l'ordre de grandeur, une bonne heuristique garde l'optimalité, une amélioration locale gagne `À MESURER` % pour un coût `À MESURER`.

**Diapo 15 — Limites et suites**

- On ignore les sens uniques (graphe non orienté) et on optimise la distance, pas le temps.
- Les demandes sont synthétiques et il n'y a pas de fenêtres horaires.
- 2-opt et Or-opt s'arrêtent sur un minimum local.
- Suites possibles : Contraction Hierarchies pour accélérer les requêtes (Geisberger et al., 2008), temps de trajet avec les vitesses OSM, recuit simulé pour sortir des minima locaux.

**Diapo 16 — Merci, questions**

- Lien du dépôt.

## Texte oral

> Pour conclure, on a une chaîne complète, de la carte OpenStreetMap jusqu'aux tournées de plusieurs camions, avec des tests de correction et des mesures. On a fait des choix de simplification, qu'on assume : on ignore les sens uniques et on optimise la distance plutôt que le temps. Pour aller plus loin, on pourrait utiliser les Contraction Hierarchies, la technique des GPS actuels, ou une métaheuristique pour sortir des minima locaux. Merci, on est prêts pour vos questions.
