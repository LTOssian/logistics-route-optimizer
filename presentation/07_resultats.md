# 7. Résultats

Orateur : Sofia — 2 min

## Message clé

Le temps et la qualité évoluent en sens inverse : chaque étape d'amélioration coûte du temps, et on peut le chiffrer.

## Protocole (une diapo, 30 s)

- Même machine, compilation `-O2` pour les benchmarks (pas `-g`).
- `CLOCK_MONOTONIC`, 5 répétitions, médiane.
- Graine fixe 42 pour les points, les paires source/cible et les demandes.
- Environnement noté dans `benchmarks/ENV.md` : CPU, OS, compilateur, date d'extraction OSM.

## Benchmarks

| Id | Question | Variables | Mesures |
|---|---|---|---|
| B1 | Plus court chemin | Dijkstra V1, Dijkstra tas, A\* ; 4 tailles de graphe ; 100 paires | temps médian, nœuds *settled* |
| B2 | Matrice | N = 10, 20, 50, 100, 200 sur le plus grand graphe | temps |
| B3 | TSP | NN, NN + 2-opt, Held-Karp (N ≤ 12) ; berlin52 | longueur, écart à l'optimum, temps |
| B4 | CVRP | C&W, C&W + Or-opt ; N = 20 à 200 ; A-n32-k5 | distance, véhicules, écart, temps |

## Diapos

**Diapo 16 — Plus court chemin (B1)**

- Courbe : temps en fonction de V, une courbe par algorithme (échelle log).
- Chiffre clé : A\* explore `À MESURER` % des nœuds de Dijkstra.

**Diapo 17 — Tournées (B3 et B4)**

- Tableau : longueur et écart à l'optimum, NN vs NN + 2-opt, C&W vs C&W + Or-opt.
- Courbe : temps en fonction de N.
- Phrase de conclusion sur le compromis qualité/temps.

## Texte oral

> Tous les temps sont des médianes sur 5 répétitions, sur la même machine, avec une graine fixe. Pour le plus court chemin, `À MESURER`. Pour les tournées, 2-opt fait passer l'écart à l'optimum de `À MESURER` à `À MESURER` %, pour un temps `À MESURER`.

## Tableau des complexités (pour le rapport et en réserve pour les questions)

| Algorithme | Temps | Mémoire |
|---|---|---|
| Dijkstra V1 | `O(V²)` | `O(V)` |
| Dijkstra tas binaire | `O((V + E) log V)` | `O(V + E)` |
| A\* | même pire cas que Dijkstra, beaucoup moins en pratique | `O(V + E)` |
| Matrice | `O(N · (V + E) log V)` | `O(N²)` |
| Plus proche voisin | `O(N²)` | `O(N)` |
| 2-opt | `O(N²)` par passe | `O(N)` |
| Held-Karp | `O(N² · 2^N)` | `O(N · 2^N)` |
| Clarke & Wright | `O(N² log N)` | `O(N²)` |
| Or-opt | `O(N²)` par passe | `O(N)` |

## À préparer

- Les 4 figures, générées par `benchmarks/plot.py` à partir des CSV.
- Ne garder que 2 figures sur les diapos, le reste va dans le rapport.

## Transition

> Plutôt que des courbes, je vous montre l'outil en direct.
