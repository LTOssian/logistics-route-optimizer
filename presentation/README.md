# Soutenance — base de travail

Soutenance notée de 15 minutes, en français, avec démo live. Ce dossier sert de base pour le texte oral et les diapos (les diapos elles-mêmes sont sur la branche `presentation`).

Chaque note suit le même format :

- **Message clé** : la phrase que le prof doit retenir.
- **Diapos** : le contenu des diapos de la partie.
- **Texte oral** : brouillon à réécrire avec nos mots.
- **À préparer** : schémas, chiffres, captures.

Les chiffres marqués `À MESURER` viennent des benchmarks (phase 7). On n'écrit aucun chiffre qui ne sort pas de `benchmarks/results/`.

## Déroulé (15 min)

| # | Partie | Durée | Fin à | Orateur |
|---|---|---|---|---|
| 1 | [[01_probleme\|Problème et modélisation]] | 1:30 | 1:30 | Sofia |
| 2 | [[02_donnees_osm\|Des données OSM au graphe]] | 1:30 | 3:00 | Sofia |
| 3 | [[03_dijkstra\|Dijkstra]] | 2:00 | 5:00 | Louisan |
| 4 | [[04_astar\|A\*]] | 1:30 | 6:30 | Louisan |
| 5 | [[05_tsp\|Matrice de distances et TSP]] | 2:00 | 8:30 | Faustine |
| 6 | [[06_cvrp\|CVRP]] | 2:00 | 10:30 | Faustine |
| 7 | [[07_resultats\|Résultats]] | 2:00 | 12:30 | Sofia |
| 8 | [[08_demo\|Démo live]] | 1:30 | 14:00 | Louisan |
| 9 | [[09_conclusion\|Conclusion et limites]] | 1:00 | 15:00 | Faustine |

Questions du prof : voir [[questions_jury]].

## Fil rouge

Une entreprise livre N clients depuis un entrepôt à Lyon. On part de la carte réelle et on remonte étage par étage :

```
OpenStreetMap -> graphe -> plus court chemin -> matrice N x N -> tournée (TSP) -> tournées multi-véhicules (CVRP)
```

Chaque étage utilise le précédent. C'est ce qui structure la présentation et le code.

## Règles pour les diapos

- Une idée par diapo, 3 à 5 puces maximum.
- Un schéma ou une courbe vaut mieux qu'un paragraphe.
- Pas de code complet : au plus 10 lignes (la boucle de Dijkstra, le gain 2-opt).
- Chaque complexité annoncée doit pouvoir être justifiée à l'oral.
- Chaque orateur passe la parole au suivant par une phrase de transition (notée en fin de note).
