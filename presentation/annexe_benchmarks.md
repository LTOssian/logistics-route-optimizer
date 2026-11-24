# Annexe — protocole et benchmarks


Les résultats ne forment plus une partie à part : chaque benchmark est montré dans la partie de son algorithme (B1 en Dijkstra et A\*, B3 en TSP, B4 en CVRP, compromis qualité/temps en conclusion). Ce fichier garde le protocole et le détail, pour l'annexe des diapos, le rapport et les questions.

## Protocole

- Même machine, compilation `-O2` pour les benchmarks (pas `-g`).
- `CLOCK_MONOTONIC`, 5 répétitions, médiane.
- Graine fixe 42 pour les points, les paires source/cible et les demandes.
- Environnement noté dans `benchmarks/ENV.md` : CPU, OS, compilateur, date d'extraction OSM.

## Benchmarks

| Id | Question | Variables | Mesures |
|---|---|---|---|
| B1 | Plus court chemin | Dijkstra V1, Dijkstra tas, A\* ; 4 tailles de graphe ; 100 paires | temps médian, nœuds *settled* |
| B2 | Matrice | N = 10, 20, 50, 100, 200 sur le plus grand graphe | temps |
| B3 | TSP | NN, NN + 2-opt, Held-Karp (N ≤ 15) ; berlin52 | longueur, écart à l'optimum, temps |
| B4 | CVRP | C&W, C&W + Or-opt ; N = 20 à 200 ; A-n32-k5 | distance, véhicules, écart, temps |

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

- Les figures, générées par `benchmarks/plot.py` à partir des CSV.
- B1 va sur la diapo 6 (et le chiffre clé sur la diapo 8), B3 sur la diapo 10, B4 sur la diapo 12, le compromis qualité/temps sur la diapo 14 (bilan). Le reste va dans le rapport.
