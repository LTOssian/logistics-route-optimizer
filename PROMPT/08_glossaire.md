# 08 — Glossaire

| Terme | Définition |
|---|---|
| **Graphe** | Ensemble de nœuds (sommets) reliés par des arêtes. |
| **Graphe orienté** | Arêtes avec un sens (A→B ≠ B→A). Modélise les sens uniques. |
| **Poids** | Coût d'une arête (distance, temps, péage). Ici `double`, en mètres. |
| **Liste d'adjacence** | Représentation où chaque nœud stocke la liste de ses arêtes sortantes. Économe pour les graphes creux. |
| **Nœud dense / ID interne** | Nœud numéroté `0..n-1` pour indexer des tableaux. Distinct de l'ID OSM. |
| **Dijkstra** | Algorithme de plus court chemin depuis une source, sans heuristique. `O((V+E) log V)` avec tas binaire. |
| **A\*** | Dijkstra + heuristique `h` : explore le nœud de plus petit `f = g + h`. |
| **`g(n)`** | Coût réel accumulé depuis le départ jusqu'à `n`. |
| **`h(n)`** | Estimation du coût restant de `n` jusqu'au but. |
| **`f(n)`** | Estimation du coût total via `n` : `f = g + h`. |
| **Heuristique admissible** | `h(n) ≤ coût réel restant`. Garantit l'optimalité de A\*. |
| **Heuristique consistante** | `h(n) ≤ coût(n→n') + h(n')`. Garantit l'optimalité sans rouvrir les nœuds fermés. |
| **Haversine** | Distance grand-cercle entre deux points lat/lon. Borne inférieure du trajet routier. |
| **Nœud *settled*** | Nœud définitivement extrait de l'open set (sa distance est définitive). Métrique de coût d'un algo. |
| **Lazy deletion** | Ne pas retirer les vieilles entrées du tas ; les ignorer au pop si le nœud est déjà fermé. |
| **`INFINITY`** | Valeur représentant un nœud inatteignable (`<math.h>`). |
| **TSP** | *Traveling Salesman Problem* : tournée passant par tous les points une fois, de longueur minimale. |
| **CVRP** | *Capacitated Vehicle Routing Problem* : TSP avec plusieurs véhicules, capacité limitée, dépôt unique. |
| **Plus proche voisin (NN)** | Heuristique constructive : aller toujours au point non visité le plus proche. `O(N²)`. |
| **2-opt** | Amélioration locale : inverser un segment de tournée pour raccourcir. `O(N²)` par passe. |
| **Or-opt** | Amélioration locale : déplacer un segment de 1 à 3 nœuds à un autre endroit. |
| **Clarke & Wright** | Heuristique des « économies » pour le VRP : fusionner des routes selon le gain décroissant. `O(N² log N)`. |
| **Held-Karp** | Programmation dynamique donnant le TSP **exact** en `O(N² · 2^N)`. Utilisable pour de petits `N`. |
| **Contraction Hierarchies (CH)** | Prétraitement (contracter les nœuds par ordre d'importance + raccourcis) puis requêtes bidirectionnelles très rapides. |
| **Witness search** | Mini-Dijkstra local qui vérifie si un raccourci est réellement nécessaire lors d'une contraction. |
| **Complexité** | Croissance du coût (temps/mémoire) avec la taille de l'entrée. |
| **Coût amorti** | Coût moyen par opération sur une séquence (ex. `O(1)` amorti pour l'ajout dans un tableau dynamique qui double). |
| **`CLOCK_MONOTONIC`** | Horloge système monotone, adaptée à la mesure de durées (benchmarks). |
| **Écart à l'optimum** | `(solution - optimum) / optimum`, en pourcentage. Mesure de qualité. |
