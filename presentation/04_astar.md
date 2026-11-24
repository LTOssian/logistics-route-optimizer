# 4. A\*

Orateur : à définir — 1 min 30 · Vocabulaire : [[glossaire]]

## Message clé

A\* explore beaucoup moins de nœuds que Dijkstra et trouve exactement la même distance, parce que l'heuristique à vol d'oiseau ne surestime jamais le trajet par la route.

## Diapos

**Diapo 7 — f = g + h**

- `g(n)` : distance réelle depuis la source.
- `h(n)` : estimation du reste, ici la distance Haversine (vol d'oiseau sur la sphère) jusqu'à la cible.
- On extrait le nœud de plus petit `f = g + h` au lieu du plus petit `g`.
- Article : Hart, Nilsson et Raphael (1968), qui prouvent l'optimalité si `h` est admissible.

**Diapo 8 — Pourquoi c'est encore optimal**

- Admissible : une route est toujours au moins aussi longue que le vol d'oiseau, donc `h(n) ≤ distance réelle restante`.
- Consistante : `h(u) ≤ w(u, v) + h(v)` (inégalité triangulaire), donc pas besoin de rouvrir un nœud fermé.
- On s'arrête quand la cible sort du tas, pas quand elle y entre.
- Vérification : sur toutes nos requêtes de test, A\* et Dijkstra trouvent la même distance (à 1e-9 près).
- Comparaison équitable : Dijkstra s'arrête lui aussi quand la cible sort du tas.
- Nœuds explorés A\* vs Dijkstra : `À MESURER` (benchmark B1).

## Texte oral

> A\*, c'est Dijkstra avec une boussole. En plus de la distance déjà parcourue, on ajoute une estimation de ce qui reste : la distance à vol d'oiseau jusqu'à la cible. Le tas favorise donc les nœuds qui vont dans la bonne direction.
>
> La question, c'est : est-ce qu'on perd l'optimalité ? Non, si l'heuristique ne surestime jamais. Et une route ne peut pas être plus courte que la ligne droite. C'est le résultat de l'article de Hart, Nilsson et Raphael en 1968. On l'a vérifié en comparant la distance trouvée par A\* et par Dijkstra sur toutes nos requêtes. Par contre, A\* explore `À MESURER` fois moins de nœuds.

## À préparer

- Image côte à côte : nœuds explorés par Dijkstra (disque) et par A\* (ellipse vers la cible). Générée en exportant les nœuds *settled* puis tracée en Python.
- Chiffre clé du benchmark B1 (nœuds *settled* moyens, temps médian).

## À vérifier

- Utiliser le même rayon terrestre qu'OSMnx (6 371 009 m) pour Haversine. Avec un rayon plus grand, l'heuristique pourrait dépasser la longueur réelle d'une rue droite et ne plus être admissible.

## Transition

> On sait aller d'un point à un autre. Maintenant on a N clients à livrer.
