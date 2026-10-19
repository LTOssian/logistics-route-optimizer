
# Contraction Hierarchies (CH) — Résumé

**Article** : _Contraction Hierarchies: Faster and Simpler Hierarchical Routing in Road Networks_ Geisberger, Sanders, Schultes, Delling (WEA 2008).

---

# Partie 1

## Le problème

Tu as une carte routière énorme et tu veux trouver le plus court chemin entre deux points **très vite**. Dijkstra classique marche, mais il explore des millions de nœuds : trop lent.

## L'idée

Comme quand tu conduis : tu quittes ton quartier, tu montes sur une grande route, tu roules sur l'autoroute, puis tu redescends vers ta destination. Pas besoin de regarder toutes les petites rues au milieu.

On fait donc ça en deux temps : un **prétraitement** (une seule fois, lent) puis des **requêtes** (rapides).

## Étape 1 : prétraitement

**a) Classer les nœuds** du moins important (petite rue) au plus important (autoroute). Chaque nœud reçoit un numéro = son **rang**.

**b) Supprimer les nœuds un par un**, du moins important au plus important, sans perdre d'information sur les distances.

Exemple : `A —2— v —3— B`. Si on supprime `v`, on ajoute un **raccourci** `A —5— B`. Les distances restent correctes.

**Sauf si c'est inutile** : s'il existe déjà un autre chemin A→B de longueur ≤ 5 qui ne passe pas par `v`, pas besoin de raccourci. Cette vérification s'appelle la **recherche de témoin** (_witness search_) : un mini Dijkstra local qui ignore `v`.

À la fin, on garde le graphe d'origine **plus tous les raccourcis créés**.

**c) Choisir qui supprimer en premier** : avec un score dans un tas min. Le terme principal est le nombre de raccourcis créés par rapport au nombre d'arêtes supprimées (moins il y en a, mieux c'est). On ajoute un bonus pour supprimer de façon répartie dans tout le graphe, et pas toujours dans le même coin.

## Étape 2 : requête

On coupe le graphe en deux :

- **Montant (G↑)** : arêtes qui vont vers un nœud de rang plus élevé
- **Descendant (G↓)** : arêtes qui vont vers un rang plus faible

Pour aller de `s` à `t` :

1. Dijkstra depuis `s` qui **ne fait que monter**.
2. Dijkstra depuis `t`, à l'envers, qui **ne fait que monter** aussi.
3. Les deux se rejoignent au nœud le plus important du chemin.
4. Distance = plus petite somme `d(s,v) + d(v,t)` parmi les nœuds `v` atteints par les deux recherches.

Comme on ne monte que vers des nœuds peu nombreux et importants, chaque recherche explore très peu de nœuds (environ 400 au lieu de millions).

## À retenir

- **Prétraitement lent, requêtes ultra rapides.**
- **Raccourci** = arête qui remplace un chemin passant par un nœud supprimé.
- **Rang** = ordre de suppression ; les requêtes ne font que monter.
- **Le plus dur** : bien choisir l'ordre de suppression. Pour commencer, utilise juste l'edge difference + le nombre de voisins déjà supprimés.
- Pour reconstruire le vrai chemin, stocke pour chaque raccourci le nœud qu'il contourne, puis déplie récursivement.

---

# Partie 2 : Détails techniques

## 1. Contraction d'un nœud

Contracter `v` = le retirer du graphe **sans changer les distances** entre les nœuds restants.

Pour chaque paire de voisins `(u, w)` avec arêtes `u→v` et `v→w` :

- Si `u → v → w` est **l'unique plus court chemin** de `u` à `w`, on ajoute un raccourci `(u, w)` de poids `c(u,v) + c(v,w)`.
- Sinon (un autre chemin aussi court existe sans passer par `v`), pas de raccourci.

### Implémentation de la witness search

- Dijkstra **depuis chaque voisin entrant `u`**, en ignorant `v`, uniquement sur les nœuds **non encore contractés**.
- On peut arrêter la recherche à la distance `c(u,v) + max c(v,w)`.
- Astuce de l'article : une recherche arrière « à un saut » depuis chaque cible `w` stocke des _buckets_ (entrées `(c(x,w), w)` sur chaque nœud `x` voisin entrant de `w`), ce qui permet de limiter encore la recherche avant.
- **Limites pour accélérer** : nombre de sauts (_hop limit_) et/ou taille de l'espace de recherche.

> **Important** : limiter la recherche ne casse **pas** la correction. Si aucun témoin n'est trouvé, on ajoute le raccourci par précaution. On peut avoir des raccourcis inutiles, jamais de résultat faux.

### Stratégie de hop limit

On commence à 1 saut, puis on augmente (1 → 2 → 3 → 5) quand le **degré moyen** du graphe résiduel dépasse un seuil (3,3 pour 1→2 ; 10 pour 2→3 et 3→5). Sans ça, le degré moyen explose pour les petites limites.

## 2. Ordre des nœuds (node ordering)

Une **file de priorité** : le nœud de plus petite priorité est contracté en premier. La priorité est une **combinaison linéaire** de plusieurs termes.

|Terme|Rôle|
|---|---|
|**Edge difference (ED)**|`nb de raccourcis ajoutés − nb d'arêtes incidentes à v`. Terme principal : on veut peu d'arêtes.|
|**Deleted neighbors (D)**|Nombre de voisins déjà contractés. Favorise une contraction **uniforme**.|
|**Voronoi regions (V)**|Racine carrée de la taille de la région de Voronoï du nœud. Même but d'uniformité.|
|**Search space size (S)**|Coût des recherches de témoin pour contracter ce nœud.|
|**Q(v)**|Borne sup. du nb de sauts d'un chemin vers `v` en requête. À la contraction de `v`, pour chaque voisin `u` : `Q(u) = max(Q(u), Q(v)+1)`.|
|**Betweenness (W)**|Centralité globale (optionnel, gain d'environ 3 %).|

**Pourquoi l'uniformité ?** Si le graphe est un simple chemin et qu'on n'utilise que l'edge difference, on obtient une hiérarchie linéaire (requêtes en temps linéaire). En contractant des **ensembles indépendants maximaux** successivement, on obtient une hiérarchie de profondeur logarithmique.

### Mises à jour des priorités

Quand on contracte `v`, les priorités des autres nœuds changent :

- **Lazy update** : avant de contracter le minimum, on recalcule sa priorité. Si elle dépasse celle du suivant, on le réinsère et on passe au suivant. On répète jusqu'à trouver un minimum cohérent.
- **Recalcul des priorités des voisins** de `v` après contraction.
- **Reconstruction périodique** de toute la file.

## 3. Résultat du prétraitement

Chaque nœud a un **rang**. Le graphe CH = graphe d'origine **+ tous les raccourcis**, séparé en :

- **G↑** : arêtes `(u, v)` avec `rang(u) < rang(v)`
- **G↓** : arêtes `(u, v)` avec `rang(u) > rang(v)`

## 4. Requête s → t

1. Recherche **avant** depuis `s` dans G↑.
2. Recherche **arrière** depuis `t` dans G↓ (arêtes remontées à l'envers, donc on monte aussi).
3. Les deux alternent et se rencontrent au nœud le plus important du plus court chemin.

**Lemme 1** : `d(s,t) = min { d(s,v) + d(v,t) : v atteint par les deux recherches }`.

### Optimisations

- **Critère d'arrêt** : une direction s'arrête quand la plus petite clé de sa file est ≥ au meilleur candidat trouvé. On n'arrête **pas** dès la première rencontre.
- **Stall-on-demand** : avant de _settle_ `v` dans la recherche avant, on regarde ses arêtes **descendantes** entrantes `(w, v)` avec `w > v`. Si `d(w) + c(w,v) < d(v)`, la distance de `v` est sous-optimale : on le « stalle » (settled, mais ses arêtes sortantes ne sont pas relâchées). Il faut gérer le « dé-stall » si un meilleur chemin est trouvé plus tard. Même principe pour la recherche arrière.

## 5. Structure de données

- Chaque arête `(u, v)` est stockée **une seule fois, au nœud de plus petit rang**, avec deux **flags de direction** (G↑, G↓ ou les deux).
- Pour les arêtes non orientées de même poids, c'est plus compact qu'un Dijkstra classique qui stocke l'arête aux deux extrémités : d'où l'**« espace négatif »** annoncé (valable sans unpacking).

### Reconstruction du chemin (unpacking)

Chaque raccourci `(u, w)` contourne **exactement un nœud** `v`. On stocke `v` avec le raccourci, puis on déplie récursivement : `(u,w)` → `(u,v)` + `(v,w)`, jusqu'aux arêtes d'origine. Coût : environ 5,8 à 10,8 octets/nœud.

## 6. Applications

- **Changement de poids** (voiture → camion) : on réutilise l'ordre des nœuds et on refait seulement la construction de la hiérarchie (~30 % plus long, requêtes quasi identiques).
- **Many-to-many** : |T| recherches arrière dont on stocke les espaces (buckets), puis |S| recherches avant.
- **Oracle de distance** : espaces de recherche stockés triés par id de nœud ; une requête = intersection de deux tableaux triés.
- **Transit-node routing**, combinaison avec A* / arc flags.
- **Mobile** : faible empreinte mémoire.

## 7. Résultats (Europe de l'Ouest, 18 M nœuds)

|Variante|Prétraitement|Requête|Nœuds settled|
|---|---|---|---|
|**Économique** (EDS1235)|~9 min (ordre 545 s + construction 57 s)|~223 µs|~459|
|**Agressive** (EVSQL)|~27 min|~159 µs|~368|
|HNR (méthode précédente)|~10 min|~802 µs|~957|

Les CH sont environ **5 fois plus rapides** que HNR, avec un surcoût d'espace négatif ou faible.