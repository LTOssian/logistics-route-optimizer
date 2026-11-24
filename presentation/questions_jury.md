# Questions probables du prof

Chaque membre doit pouvoir répondre à toutes ces questions, pas seulement sur sa partie. Réponses courtes, à compléter.

## Plus court chemin

**Pourquoi Dijkstra ne marche pas avec des poids négatifs ?**
L'invariant « un nœud extrait a sa distance définitive » tombe : un chemin plus long en nombre d'arêtes pourrait devenir plus court plus tard. Il faudrait Bellman-Ford. Nos poids sont des longueurs, donc toujours positifs.

**Pourquoi la lazy deletion plutôt que decrease-key ?**
Un tas binaire simple ne sait pas retrouver un élément. Decrease-key demande de maintenir la position de chaque nœud dans le tas. Avec la lazy deletion, on réinsère et on ignore les entrées périmées au pop. Le tas peut contenir jusqu'à E entrées, ce qui donne `O(E log E) = O(E log V)` : même ordre de grandeur.

**Pourquoi Haversine est admissible ?**
La longueur d'une rue est calculée le long de sa géométrie sur la sphère. Elle est donc toujours au moins égale à la distance grand-cercle entre ses extrémités. Par inégalité triangulaire, tout chemin est au moins aussi long que le vol d'oiseau.

**Pourquoi s'arrêter au pop de la cible et pas à son insertion ?**
À l'insertion, on a un chemin vers la cible, mais pas forcément le meilleur. Au pop, tous les chemins restants ont un `f` supérieur ou égal, donc aucun ne peut être plus court.

**A\* est-il toujours plus rapide ?**
Non. Au pire cas, il explore autant que Dijkstra, par exemple si `h = 0` ou si le graphe force de grands détours. Il calcule aussi Haversine à chaque nœud, ce qui coûte.

## Tournées

**Pourquoi des heuristiques ?**
TSP et CVRP sont NP-difficiles : pas d'algorithme polynomial connu. Held-Karp est exact mais exponentiel, donc limité à 15 points chez nous : sa mémoire grandit en `N · 2^N`.

**2-opt donne-t-il l'optimum ?**
Non. Il s'arrête sur un minimum local : plus aucun échange de deux arêtes n'améliore, mais un changement plus grand le pourrait. On mesure l'écart sur les petites instances et sur berlin52.

**Pourquoi le graphe non orienté ?**
Pour avoir une matrice symétrique. 2-opt inverse un segment de la tournée : avec une matrice asymétrique, il faudrait recalculer tout le segment pour chaque échange. C'est une simplification, citée dans les limites.

**D'où vient l'idée des économies de Clarke & Wright ?**
Servir `i` puis `j` dans la même tournée évite le retour `i → 0` et l'aller `0 → j`, et ajoute le trajet `i → j`. On fusionne d'abord les paires qui économisent le plus.

## Méthode

**Comment vous savez que votre code est juste ?**
Tests unitaires par module, oracle exhaustif pour Dijkstra, même distance pour A\* et Dijkstra (à 1e-9 près), Held-Karp pour le TSP, contrôles de faisabilité pour le CVRP. `make test` passe, et valgrind ne signale pas de fuite.

**Pourquoi C ?**
C'est imposé par le sujet. Ça nous oblige à gérer la mémoire et les structures nous-mêmes, et les temps mesurés ne sont pas brouillés par un runtime.

**Les résultats sont-ils reproductibles ?**
Graine fixe, date d'extraction OSM et version d'OSMnx notées, environnement documenté, script de données versionné.

**Qu'apporteraient les Contraction Hierarchies ?**
Un prétraitement qui ajoute des raccourcis. Ensuite, chaque requête explore quelques centaines de nœuds au lieu de milliers. C'est utile quand on fait beaucoup de requêtes sur la même carte, comme pour la matrice de distances.
