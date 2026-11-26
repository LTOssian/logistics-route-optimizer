# Glossaire

Les termes utilisés dans la présentation et les diapos, dans l'ordre où ils apparaissent. Chaque définition doit pouvoir se redire à l'oral en une phrase.

## Graphes et données

**Graphe** : un ensemble de points (les *nœuds*) reliés par des liens (les *arêtes*). Ici, les nœuds sont les intersections et les arêtes sont les rues.

**V, E** : le nombre de nœuds (*vertices*) et le nombre d'arêtes (*edges*) du graphe. Les complexités s'expriment avec ces deux lettres.

**Poids** : le coût pour emprunter une arête. Chez nous, c'est la longueur de la rue en mètres, toujours positive.

**Graphe orienté / non orienté** : dans un graphe orienté, une arête a un sens (A → B ne veut pas dire B → A), ce qui permet de modéliser les sens uniques. Dans un graphe non orienté, on peut parcourir chaque rue dans les deux sens. On a choisi le non orienté pour simplifier.

**Graphe creux** : un graphe qui a peu d'arêtes par rapport au maximum possible. Un réseau routier en est un, avec environ 3 rues par nœud.

**Liste d'adjacence** : pour chaque nœud, on stocke la liste de ses voisins directs. Ça prend une mémoire proportionnelle à V + E, ce qui convient aux graphes creux.

**Matrice d'adjacence** : un tableau V × V qui dit, pour chaque paire de nœuds, s'il existe une arête entre eux. Elle prend V² cases, ce qui est trop pour un réseau routier.

**Composante connexe** : un morceau du graphe dans lequel tous les nœuds sont joignables entre eux. On ne garde que la plus grande, sinon certaines livraisons seraient impossibles.

**OpenStreetMap (OSM)** : une carte du monde libre et collaborative, de type Wikipédia.

**OSMnx** : une bibliothèque Python qui télécharge le réseau routier OSM d'une zone et le transforme en graphe.

**Identifiants denses** : des numéros de nœuds qui se suivent, de 0 à n−1. Les identifiants OSM sont grands et irréguliers, donc on les renumérote pour pouvoir utiliser `dist[i]` comme un simple tableau.

**Overpass** : le serveur qui répond aux requêtes sur les données OpenStreetMap. OSMnx l'interroge pour télécharger une zone.

**Réseau « drive »** : la partie du réseau OSM ouverte aux voitures. Elle exclut les rues piétonnes, les pistes cyclables et les chemins.

**Boucle** : une arête qui part d'un nœud et y revient (un rond-point en impasse, par exemple). Elle ne raccourcit jamais un chemin, on la retire.

**Arêtes parallèles** : plusieurs rues qui relient les deux mêmes carrefours. Pour un plus court chemin, seule la plus courte compte.

**Données gelées** : des données qu'on ne régénère plus, pour que tout le monde travaille sur les mêmes nœuds et que les chiffres des diapos restent vrais.

**CSV** : un fichier texte avec une valeur par colonne, séparées par des virgules. C'est le format d'échange entre le script Python et le programme C.

## Plus court chemin

**Plus court chemin** : le chemin dont la somme des poids est la plus petite entre deux nœuds.

**Dijkstra** : l'algorithme qui calcule les plus courts chemins depuis une source quand les poids sont positifs. À chaque étape, il traite le nœud non traité le plus proche de la source.

**Relâcher une arête** : vérifier si passer par le nœud courant `u` raccourcit le chemin vers son voisin `v`. Si oui, on met à jour `dist[v]` et `prev[v]`.

**`dist[]` et `prev[]`** : `dist[v]` est la meilleure distance connue de la source à `v`. `prev[v]` est le nœud juste avant `v` sur ce chemin, ce qui permet de reconstruire le chemin en remontant depuis la cible.

**`INFINITY`** : la valeur « infini » du C (`<math.h>`). Elle désigne un nœud qu'on n'a pas encore atteint, ou qu'on ne peut pas atteindre.

**Invariant** : une propriété toujours vraie à chaque étape de l'algorithme. Celui de Dijkstra : quand un nœud est extrait, sa distance est définitive.

**Nœud *settled* (traité)** : un nœud dont la distance est définitive. Leur nombre mesure le travail d'un algorithme, indépendamment de la machine.

**Tas binaire** : une structure de données qui donne toujours le plus petit élément en `O(1)`. Insérer un élément ou retirer le plus petit coûte `O(log n)`. Dijkstra s'en sert pour trouver vite le prochain nœud à traiter.

**Decrease-key** : l'opération qui diminue la valeur d'un élément déjà présent dans le tas. Elle oblige à savoir où se trouve chaque nœud dans le tas.

**Lazy deletion** (suppression paresseuse) : au lieu de faire un decrease-key, on réinsère le nœud avec sa nouvelle distance. Quand on retire du tas une ancienne entrée devenue fausse, on l'ignore. C'est plus simple à coder.

**Oracle exhaustif** : un programme lent mais sûr, qui essaie tous les chemins possibles sur un petit graphe. On compare ses réponses à celles de Dijkstra pour vérifier que Dijkstra est juste.

**Chemin simple** : un chemin qui ne passe jamais deux fois par le même nœud.

**CLRS** : le manuel d'algorithmique de Cormen, Leiserson, Rivest et Stein. Notre graphe jouet de 5 nœuds vient de ce livre.

## A\*

**A\* (« A étoile »)** : une variante de Dijkstra qui privilégie les nœuds qui vont vers la cible. Il traite d'abord le nœud de plus petit `f = g + h`.

**`g(n)`** : la distance réellement parcourue de la source jusqu'à `n`.

**`h(n)` (heuristique)** : une estimation de la distance qui reste de `n` jusqu'à la cible. Chez nous, c'est le vol d'oiseau.

**`f(n)`** : l'estimation de la longueur totale d'un chemin qui passe par `n`, `f = g + h`.

**Heuristique** : une règle d'estimation rapide, qui n'est pas forcément exacte mais qui guide la recherche.

**Haversine** : la formule de la distance entre deux points GPS (latitude, longitude) en suivant la surface de la Terre. C'est le vol d'oiseau sur une sphère.

**Distance grand-cercle** : la distance la plus courte entre deux points d'une sphère. C'est ce que calcule Haversine.

**Admissible** : se dit d'une heuristique qui ne surestime jamais la distance restante. Avec une heuristique admissible, A\* trouve le vrai plus court chemin.

**Consistante** : pour toute arête `u → v`, `h(u) ≤ poids(u, v) + h(v)`. C'est une condition plus forte que l'admissibilité, qui garantit qu'on n'a jamais besoin de revenir sur un nœud déjà traité.

**Réouverture** : remettre dans le tas un nœud déjà traité parce qu'on lui a trouvé un meilleur chemin. Avec une heuristique consistante, ça n'arrive jamais.

**Inégalité triangulaire** : passer par un point intermédiaire n'est jamais plus court que d'aller tout droit, soit `d(a, c) ≤ d(a, b) + d(b, c)`.

**Borne inférieure** : une valeur dont on est sûr qu'elle est inférieure ou égale à la vraie. Le vol d'oiseau est une borne inférieure de la distance par la route.

## Matrice et tournées

**Matrice de distances** : le tableau de la distance par la route entre chaque paire de points de livraison, dépôt compris. Avec N clients, elle fait (N+1) × (N+1).

**Matrice dense** : on stocke toutes les cases, même s'il y en a beaucoup. C'est possible ici parce qu'on n'a que quelques centaines de points.

**Matrice symétrique** : `d[i][j] = d[j][i]`. C'est le cas parce que notre graphe est non orienté.

**Arrêt anticipé** : on arrête Dijkstra dès que tous les points qui nous intéressent sont extraits du tas (leur distance est alors définitive), sans finir toute la ville.

**Dépôt** : l'entrepôt d'où partent les véhicules et où ils reviennent.

**TSP (*Traveling Salesman Problem*, voyageur de commerce)** : trouver la tournée la plus courte qui passe une fois par chaque point et revient au départ.

**NP-difficile** : une classe de problèmes pour lesquels on ne connaît aucun algorithme rapide (polynomial) qui donne toujours la meilleure solution. Le TSP et le CVRP en font partie, d'où l'usage d'heuristiques.

**Heuristique constructive** : une méthode qui construit une solution d'un coup, rapidement, sans chercher à l'améliorer ensuite. Le plus proche voisin et Clarke & Wright en sont.

**Plus proche voisin** : partir du dépôt et aller toujours au point non visité le plus proche.

**Amélioration locale** : partir d'une solution et la modifier par petites retouches, en ne gardant que celles qui raccourcissent. 2-opt et Or-opt en sont.

**2-opt** : retirer deux trajets de la tournée et reconnecter les morceaux autrement, en inversant le segment entre les deux. Ça supprime notamment les croisements.

**Passe** : un parcours complet de toutes les retouches possibles. On recommence les passes jusqu'à ce qu'aucune retouche n'améliore.

**Minimum local** : une solution qu'aucune petite retouche ne peut améliorer, mais qui n'est pas forcément la meilleure possible.

**Held-Karp** : un algorithme exact pour le TSP, qui stocke les meilleurs sous-chemins pour ne pas les recalculer. Il est exponentiel (`O(N² · 2^N)`), donc limité à 15 points chez nous.

**Programmation dynamique** : résoudre un problème en combinant les solutions de sous-problèmes plus petits, calculées une seule fois et stockées.

**TSPLIB / CVRPLIB** : des bibliothèques publiques d'instances de test dont la meilleure solution est connue. Elles servent à mesurer un écart à l'optimum.

**berlin52** : une instance TSPLIB de 52 lieux à Berlin.

**A-n32-k5** : une instance CVRPLIB de 32 points (dépôt compris) qui demande au moins 5 véhicules.

## CVRP

**CVRP (*Capacitated Vehicle Routing Problem*)** : répartir les clients entre plusieurs véhicules de capacité limitée, avec des tournées qui partent du dépôt et y reviennent, en minimisant la distance totale.

**Demande** : la quantité à livrer à un client (le nombre de colis). Chez nous, elle est tirée au hasard entre 1 et 10.

**Capacité (Q)** : la quantité maximale qu'un véhicule peut transporter.

**Clarke & Wright** : l'heuristique de 1964 qui part d'un aller-retour par client, puis fusionne les tournées dans l'ordre des plus grosses économies.

**Économie (*saving*)** : `s(i, j) = d(0, i) + d(0, j) − d(i, j)`, la distance gagnée en servant `i` et `j` dans la même tournée plutôt qu'avec deux allers-retours.

**Or-opt** : déplacer un petit bloc de 1 à 3 clients consécutifs à un autre endroit, dans la même tournée ou dans une autre.

**Fenêtre horaire** : un créneau pendant lequel un client doit être livré. On ne le gère pas.

## Mesures

**Complexité (`O(...)`)** : comment le temps ou la mémoire grandit avec la taille de l'entrée. `O(V²)` veut dire que si V double, le temps est multiplié par environ 4.

**Pire cas** : le temps maximal sur l'entrée la plus défavorable possible.

**Benchmark** : une mesure de performance faite selon un protocole fixe, pour pouvoir comparer.

**Écart à l'optimum** : `(notre solution − meilleure solution) / meilleure solution`, en pourcentage. 0 % veut dire optimal.

**Graine (*seed*)** : le nombre qui initialise le générateur aléatoire. Avec la même graine, on obtient les mêmes points et les mêmes demandes à chaque fois.

**Reproductibilité** : n'importe qui peut relancer l'expérience et obtenir les mêmes résultats.

**Médiane** : la valeur du milieu quand on trie les mesures. Elle est moins sensible qu'une moyenne à une mesure ratée.

**`CLOCK_MONOTONIC`** : une horloge du système qui avance toujours, même si l'heure de l'ordinateur change. On l'utilise pour mesurer des durées.

**`-O2`** : une option du compilateur qui active les optimisations. On l'utilise pour les mesures, et `-g` (débogage) pour le développement.

## Outils

**valgrind** : un outil qui détecte les fuites mémoire et les accès mémoire invalides d'un programme C.

**Fuite mémoire** : une mémoire allouée avec `malloc` et jamais libérée avec `free`.

**Docker** : un outil qui fait tourner le programme dans un environnement identique sur toutes les machines (même compilateur, mêmes versions).

**CI (intégration continue)** : GitHub compile et lance les tests automatiquement à chaque pull request, avant le merge.

**Marp** : un outil qui transforme un fichier Markdown en diapos (PDF ou HTML).

## Pour aller plus loin

**Métaheuristique** : une stratégie générale pour sortir des minima locaux, par exemple en acceptant parfois une solution moins bonne.

**Recuit simulé** : une métaheuristique qui accepte parfois une retouche qui dégrade la solution, de moins en moins souvent au fil du temps.

**Contraction Hierarchies** : une technique des GPS modernes. On prépare le graphe une fois en ajoutant des raccourcis, ensuite chaque requête explore très peu de nœuds.

**IMRaD** : le plan standard d'un rapport scientifique : Introduction, Méthodes, Résultats et Discussion.
