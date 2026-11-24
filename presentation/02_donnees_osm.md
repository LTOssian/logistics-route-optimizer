# 2. Des données OSM au graphe

Orateur : à définir — 1 min 30

## Message clé

On récupère le vrai réseau routier de Lyon avec un script Python, et le C ne lit qu'un CSV simple avec des identifiants de 0 à n−1.

## Diapos

**Diapo 4 — D'OpenStreetMap au CSV**

- OSMnx télécharge les routes carrossables (`network_type="drive"`) dans un disque autour de la place Bellecour.
- On garde la plus grande composante connexe, pour que tous les points soient joignables.
- Les identifiants OSM (grands, non consécutifs) sont renumérotés de 0 à n−1 en Python.
- Export : `nodes.csv` (`id,lat,lon`), `edges.csv` (`u,v,weight` en mètres), `meta.txt` (date d'extraction, version d'OSMnx).

**Diapo 5 — Le graphe en C**

- Liste d'adjacence : chaque nœud a la liste de ses arêtes sortantes.
- Pourquoi pas une matrice V × V : un graphe routier est creux (2 à 3 rues par intersection). Pour 10 000 nœuds, une matrice de `double` prendrait 800 Mo.
- Graphe non orienté : chaque rue est ajoutée dans les deux sens.
- Tailles utilisées pour les mesures :

| Rayon | Nœuds | Arêtes |
|---|---|---|
| 500 m | `À MESURER` | `À MESURER` |
| 1000 m | `À MESURER` | `À MESURER` |
| 2000 m | `À MESURER` | `À MESURER` |
| 4000 m | `À MESURER` | `À MESURER` |

## Texte oral

> Pour les données, on n'a pas écrit de parseur OpenStreetMap en C : ce serait un projet à lui seul. On utilise OSMnx en Python, qui télécharge les routes autour d'un point et les simplifie en intersections et tronçons. On renumérote les nœuds de 0 à n−1, ce qui permet au C d'utiliser de simples tableaux indexés par le numéro du nœud.
>
> Côté C, on stocke le graphe en liste d'adjacence, parce qu'un réseau routier est très creux. Pour faire varier la taille, on prend des disques de rayon croissant, de 500 mètres à 4 kilomètres.

## À préparer

- Capture du réseau tracé par OSMnx (`ox.plot_graph`) pour le rayon de 2000 m.
- Remplir le tableau des tailles à partir de `meta.txt`.

## À vérifier

- La date d'extraction et la version d'OSMnx sont écrites dans `meta.txt` : les données OSM changent avec le temps.
- Le nombre de nœuds et d'arêtes lu par le C est identique à celui de Python.

## Transition

> Maintenant qu'on a le graphe, voyons comment on y calcule un plus court chemin.
