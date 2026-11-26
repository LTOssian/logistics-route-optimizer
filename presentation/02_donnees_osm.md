# 2. Des données OSM au graphe

Orateur : à définir — 1 min 30 · Vocabulaire : [[glossaire]]

## Message clé

On récupère le vrai réseau routier de Lyon avec un script Python, et le C ne lit qu'un CSV simple avec des identifiants de 0 à n−1.

## Diapos

**Diapo 3 — D'OpenStreetMap au CSV**

- OSMnx télécharge les routes carrossables (`network_type="drive"`) dans un carré centré sur la place Bellecour (demi-côté R).
- On garde la plus grande composante connexe, pour que tous les points soient joignables.
- Les identifiants OSM (grands, non consécutifs) sont renumérotés de 0 à n−1 en Python.
- Export : `nodes.csv` (`id,lat,lon`), `edges.csv` (`u,v,weight` en mètres), `meta.txt` (date d'extraction, version d'OSMnx).

**Diapo 4 — Le graphe en C**

- Liste d'adjacence : chaque nœud a la liste de ses arêtes sortantes.
- Pourquoi pas une matrice V × V : un graphe routier est creux (environ 3 rues par nœud : 2,9 à 3,2 selon la taille). Pour nos 5 344 nœuds, une matrice de `double` prendrait 228 Mo, contre moins de 1 Mo en liste d'adjacence.
- Graphe non orienté : chaque rue est ajoutée dans les deux sens.
- Tailles utilisées pour les mesures :

| Demi-côté R | Nœuds | Rues | Arêtes orientées (2 × rues) |
|---|---|---|---|
| 500 m | 154 | 224 | 448 |
| 1000 m | 550 | 850 | 1 700 |
| 2000 m | 1 886 | 3 021 | 6 042 |
| 4000 m | 5 344 | 8 128 | 16 256 |

Source : `data/osm/lyon_<R>/meta.txt`, extraction du 26 novembre 2026, OSMnx 2.2.0 (PR #18).

## Texte oral

> Pour les données, on n'a pas écrit de parseur OpenStreetMap en C : ce serait un projet à lui seul. On utilise OSMnx en Python, qui télécharge les routes autour d'un point et les simplifie en intersections et tronçons. On renumérote les nœuds de 0 à n−1, ce qui permet au C d'utiliser de simples tableaux indexés par le numéro du nœud.
>
> Côté C, on stocke le graphe en liste d'adjacence, parce qu'un réseau routier est très creux. Pour faire varier la taille, on prend des carrés de plus en plus grands autour du centre, de 500 mètres à 4 kilomètres de demi-côté.

## À préparer

- [x] Capture du réseau pour R = 2000 m : `slides/img/lyon_2000.png` (branche `presentation`). Aussi `lyon_500.png` et `lyon_4000.png`.
- [x] Tableau des tailles rempli à partir de `meta.txt`.
- Si on demande « pourquoi si peu de nœuds à 500 m ? » : le réseau « drive » exclut les rues piétonnes, nombreuses autour de Bellecour et de la Presqu'île.

## À vérifier

- [x] La date d'extraction et la version d'OSMnx sont écrites dans `meta.txt` : les données OSM changent avec le temps.
- [x] Le nombre de nœuds et d'arêtes lu par le C est identique à celui de Python.
- [x] Les poids et les coordonnées sont exportés en pleine précision, sans arrondi.
- [x] Avec le Haversine du C, aucune arête n'est plus courte que le vol d'oiseau (PR #18).

## Transition

> Maintenant qu'on a le graphe, voyons comment on y calcule un plus court chemin.
