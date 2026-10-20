# 06 — Références

> [!info] Article à lire et présenter
> La consigne demande de lire **au moins un** article et d'en présenter les idées principales dans le rapport. Trois résumés existent déjà dans `etudes/`. Pour implémenter le VRP, il faudra ajouter un résumé de **Clarke & Wright**.

## Articles

### 1. Dijkstra (1959)
Dijkstra, E. W. (1959). *A note on two problems in connexion with graphs.* Numerische Mathematik, 1, 269–271.

- Plus court chemin (et arbre couvrant minimal).
- Notes : [`../etudes/djikstra.md`](../etudes/djikstra.md)

### 2. Hart, Nilsson & Raphael (1968)
Hart, P. E., Nilsson, N. J., & Raphael, B. (1968). *A formal basis for the heuristic determination of minimum cost paths.* IEEE Transactions on Systems Science and Cybernetics, 4(2), 100–107.

- Article original de A\* ; heuristique admissible, optimalité.
- Notes : [`../etudes/a_star.md`](../etudes/a_star.md)

### 3. Clarke & Wright (1964)
Clarke, G., & Wright, J. W. (1964). *Scheduling of vehicles from a central depot to a number of delivery points.* Operations Research, 12(4), 568–581.

- Heuristique des **économies** (savings) pour le VRP.
- Notes : **à rédiger** (`etudes/clarke_wright.md`) avant la phase 6.

### 4. Geisberger et al. (2008)
Geisberger, R., Sanders, P., Schultes, D., & Delling, D. (2008). *Contraction hierarchies: Faster and simpler hierarchical routing in road networks.* In WEA 2008, LNCS 5038, 319–333.

- Technique des GPS modernes : prétraitement + requêtes bidirectionnelles.
- Notes : [`../etudes/Contraction Hierarchies.md`](../etudes/Contraction Hierarchies.md)

### 5. Bast et al. (2016)
Bast, H., Delling, D., Goldberg, A., Müller-Hannemann, M., Pajor, T., Sanders, P., Wagner, D., & Werneck, R. F. (2016). *Route planning in transportation networks.* In Algorithm Engineering, LNCS 9220, 19–80.

- Synthèse des techniques de routing (Dijkstra, A\*, CH, ALT, etc.).
- Notes : à rédiger si l'article est choisi pour la présentation.

## Correspondance article → phase du projet

| Article | Phase concernée |
|---|---|
| Dijkstra (1959) | Phase 1 |
| Hart, Nilsson & Raphael (1968) | Phase 2 |
| Clarke & Wright (1964) | Phase 6 |
| Geisberger et al. (2008) | Bonus (CH) |
| Bast et al. (2016) | Cadre général / rapport |

## Bonnes pratiques de citation

- Citer la source complète (auteurs, année, titre, revue/conférence).
- Relier chaque idée de l'article à un **choix d'implémentation** ou un **résultat** du projet.
- Ne pas recopier l'article : résumer et discuter.
