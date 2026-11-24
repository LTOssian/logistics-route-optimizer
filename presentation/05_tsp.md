# 5. Matrice de distances et TSP

Orateur : à définir — 2 min

## Message clé

On construit une tournée rapide avec le plus proche voisin, puis 2-opt la raccourcit. On mesure l'écart à l'optimum exact sur les petites instances.

## Diapos

**Diapo 11 — Matrice de distances**

- Points : le dépôt et N clients tirés au hasard parmi les nœuds du graphe (graine fixe).
- Un Dijkstra par point, qu'on arrête dès que tous les autres points sont atteints.
- Matrice dense `N × N`, symétrique car le graphe est non orienté.
- Complexité : `O(N · (V + E) log V)`.

**Diapo 12 — Plus proche voisin puis 2-opt**

- Tournée fermée : départ et retour au dépôt.
- Plus proche voisin : aller toujours au client non visité le plus proche. `O(N²)`.
- 2-opt : on retire deux arêtes `(a, b)` et `(c, d)`, on reconnecte en `(a, c)` et `(b, d)` en inversant le segment entre les deux. On garde le changement si `d(a,b) + d(c,d) > d(a,c) + d(b,d)`. On recommence jusqu'à ce qu'aucun échange n'améliore. `O(N²)` par passe.
- Schéma : une tournée qui se croise, puis décroisée.

**Diapo 13 — Est-ce que c'est bon ?**

- Référence exacte : Held-Karp (programmation dynamique, `O(N² · 2^N)`), possible jusqu'à environ 12 à 15 points.
- Instance publique `berlin52` (TSPLIB), dont l'optimum est publié.
- Écart à l'optimum NN et NN + 2-opt : `À MESURER` (benchmark B3).

## Texte oral

> Pour planifier une tournée, on a besoin de la distance entre chaque paire de points. On lance un Dijkstra par point et on l'arrête dès qu'il a atteint tous les autres, ça évite d'explorer toute la ville.
>
> Le TSP est NP-difficile, donc on ne cherche pas l'optimum sur 100 points. On construit une première tournée avec le plus proche voisin : c'est rapide mais ça fait souvent des croisements. Ensuite 2-opt supprime ces croisements : on enlève deux arêtes, on reconnecte autrement, et on garde si c'est plus court.
>
> Pour savoir si c'est bon, on calcule l'optimum exact avec Held-Karp sur des petites instances, et on teste aussi sur berlin52, une instance classique dont l'optimum est connu. On obtient `À MESURER` % d'écart après 2-opt.

## À préparer

- Schéma 2-opt (avant / après décroisement).
- Tracé d'une tournée NN puis NN + 2-opt sur la carte de Lyon.
- Tableau B3 (longueur, écart, temps).

## À vérifier

- L'optimum de `berlin52` (7542 d'après TSPLIB) : relire la valeur sur le site TSPLIB avant de l'écrire sur la diapo.
- 2-opt suppose une matrice symétrique : c'est pourquoi on a choisi un graphe non orienté (voir PLAN.md, phase 3).

## Transition

> Un seul camion ne suffit pas : chaque camion a une capacité. C'est le CVRP.
