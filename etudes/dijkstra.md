

> Article : *Dijkstra (1959), A Note on Two Problems in Connexion with Graphs*

## Deux problèmes traités

### 1. Arbre couvrant minimal (_Minimum Spanning Tree_)

Construire l'arbre de longueur totale minimale reliant _n_ sommets (nœuds).

### 2. Plus court chemin entre deux nœuds (_Shortest Path_)

Trouver le chemin de longueur totale minimale entre deux sommets donnés P et Q.

C'est de ce second problème que naît l'algorithme de Dijkstra.

## Principe commun : classification en trois ensembles

**Branches (arêtes)**

|Ensemble|Signification|
|---|---|
|**I**|branches assignées à l'arbre (ou au chemin)|
|**II**|branches candidates (une par nœud de B, la meilleure connue)|
|**III**|branches restantes|

**Nœuds**

| Ensemble | Signification                                             |
| -------- | --------------------------------------------------------- |
| **A**    | nœuds traités, leur place dans la solution est définitive |
| **B**    | nœuds voisins de A, candidats pour la prochaine étape     |
| **C**    | nœuds pas encore atteints                                 |

Au départ, tous les nœuds sont dans C et toutes les branches dans III. On choisit un nœud de départ, qui passe dans A, et ses branches deviennent candidates.

## Problème 1 : arbre couvrant minimal

À chaque étape :

1. Parmi les branches de II, prendre la **plus courte** et la faire passer dans I ; son nœud de B passe dans A.
2. Pour le nœud _r_ qui vient d'entrer dans A, examiner chaque branche qui le relie à un nœud _s_ :
    - _s_ dans A : la branche est ignorée ;
    - _s_ dans B : si la nouvelle branche est plus courte que la branche candidate actuelle vers _s_, elle la remplace, sinon elle est ignorée ;
    - _s_ dans C : _s_ passe dans B et la branche entre dans II.
3. Répéter jusqu'à ce que tous les nœuds soient dans A.

## Problème 2 : plus court chemin de P à Q

Même squelette, avec un autre critère de choix. On part de P ; pour chaque nœud de B, on retient la **distance totale depuis P**. À chaque étape :

- le nœud de B dont la distance totale depuis P est **la plus petite** passe dans A ;
- on met à jour ses voisins comme ci-dessus, en comparant des distances depuis P et non de simples longueurs de branche ;
- on s'arrête dès que Q entre dans A, et on lit le chemin en remontant les branches de I depuis Q.

Le nœud choisi est forcément atteint par son plus court chemin, ce qui justifie de le figer.
