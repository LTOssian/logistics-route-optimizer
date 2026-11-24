# Soutenance — base de travail

Soutenance notée de 15 minutes, en français, avec démo live. Ce dossier sert de base pour le texte oral et les diapos (les diapos elles-mêmes sont sur la branche `presentation`).

Chaque note suit le même format :

- **Message clé** : la phrase que le prof doit retenir.
- **Diapos** : le contenu des diapos de la partie.
- **Texte oral** : brouillon à réécrire avec nos mots.
- **À préparer** : schémas, chiffres, captures.

Les chiffres marqués `À MESURER` viennent des benchmarks (phase 7). On n'écrit aucun chiffre qui ne sort pas de `benchmarks/results/`.

## Déroulé (cible 13:30, marge 1:30 pour les transitions et les imprévus)

| # | Partie | Diapos | Durée | Fin à |
|---|---|---|---|---|
| 1 | [[01_probleme\|Problème et modélisation]] | 1–2 | 1:15 | 1:15 |
| 2 | [[02_donnees_osm\|Des données OSM au graphe]] | 3–4 | 1:30 | 2:45 |
| 3 | [[03_dijkstra\|Dijkstra]] | 5–6 | 2:00 | 4:45 |
| 4 | [[04_astar\|A\*]] | 7–8 | 1:30 | 6:15 |
| 5 | [[05_tsp\|Matrice et TSP]] | 9–10 | 2:00 | 8:15 |
| 6 | [[06_cvrp\|CVRP]] | 11–12 | 2:00 | 10:15 |
| 7 | [[07_demo\|Démo live]] | 13 | 1:45 | 12:00 |
| 8 | [[08_conclusion\|Bilan, limites]] | 14–16 | 1:30 | 13:30 |

Orateurs : à définir (chacun présente ce qu'il a codé). Les résultats sont montrés dans la partie de chaque algorithme ; le protocole et le détail des mesures sont dans [[annexe_benchmarks]] (diapos d'annexe, pour les questions).

Questions du prof : voir [[questions_jury]]. Vocabulaire : [[glossaire]].

## Fil rouge

Une entreprise livre N clients depuis un entrepôt à Lyon. On part de la carte réelle et on remonte étage par étage :

```
OpenStreetMap -> graphe -> plus court chemin -> matrice (N+1) x (N+1) -> tournée (TSP) -> tournées multi-véhicules (CVRP)
```

Chaque étage utilise le précédent. C'est ce qui structure la présentation et le code.

## Règles pour les diapos

- Une idée par diapo, 3 à 5 puces maximum.
- Un schéma ou une courbe vaut mieux qu'un paragraphe.
- Pas de code complet : au plus 10 lignes (la boucle de Dijkstra, le gain 2-opt).
- Chaque complexité annoncée doit pouvoir être justifiée à l'oral.
- Chaque orateur passe la parole au suivant par une phrase de transition (notée en fin de note).
