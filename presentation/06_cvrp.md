# 6. CVRP — plusieurs véhicules à capacité limitée

Orateur : à définir — 2 min

## Message clé

Clarke & Wright fusionne des allers-retours en tournées tant que la capacité le permet. Or-opt améliore ensuite en déplaçant de petits groupes de clients.

## Diapos

**Diapo 14 — Clarke & Wright (1964)**

- Départ : un aller-retour dépôt → client → dépôt par client.
- Économie de servir `i` et `j` dans la même tournée : `s(i, j) = d(0, i) + d(0, j) − d(i, j)`.
- On trie les économies par ordre décroissant et on fusionne deux tournées si `i` et `j` sont à leurs extrémités et que la charge totale ne dépasse pas Q.
- Complexité : `O(N² log N)` (le tri domine).
- Schéma : deux allers-retours fusionnés en une tournée.

**Diapo 15 — Or-opt et vérifications**

- Or-opt : déplacer un segment de 1 à 3 clients ailleurs, dans la même tournée ou dans une autre si la capacité le permet. On garde si la distance totale baisse.
- Objectif : distance totale minimale. Le nombre de véhicules est un résultat, pas une contrainte.
- Contrôles automatiques : chaque client servi une fois, capacité respectée, toutes les tournées partent du dépôt et y reviennent.
- Instance publique `A-n32-k5` (CVRPLIB), dont l'optimum est publié. Écart : `À MESURER` (benchmark B4).

## Texte oral

> Avec plusieurs camions, il faut décider qui livre qui. L'heuristique de Clarke et Wright part d'une solution absurde : un camion par client, qui fait l'aller-retour. Ensuite on se demande combien on économise si un même camion enchaîne deux clients i et j : on évite un retour et un aller, et on ajoute le trajet de i à j. On trie ces économies et on fusionne les tournées dans cet ordre, tant que le camion n'est pas plein.
>
> Ensuite Or-opt déplace des petits groupes de un à trois clients vers une meilleure position. Les demandes des clients sont tirées au hasard avec une graine fixe, pour que les résultats soient reproductibles. On vérifie automatiquement que chaque client est servi une fois et que la capacité est respectée.

## À préparer

- Schéma de l'économie `s(i, j)` (triangle dépôt, i, j).
- Carte de Lyon avec les tournées en couleurs.
- Tableau B4 (distance C&W, C&W + Or-opt, nombre de véhicules, écart à l'optimum sur A-n32-k5).

## À vérifier

- L'optimum de `A-n32-k5` (784 d'après CVRPLIB) : relire la valeur sur le site avant de l'écrire.
- Paramètres par défaut : demandes entières entre 1 et 10, Q = 60, graine 42. Les reporter tels quels dans les diapos si on les change.

## Transition

> Voyons maintenant ce que tout ça donne en chiffres.
