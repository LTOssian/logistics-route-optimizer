# 3. Dijkstra

Orateur : Louisan — 2 min

## Message clé

Dijkstra donne le plus court chemin exact quand les poids sont positifs. La structure de données qui choisit le prochain nœud fait toute la différence de performance.

## Diapos

**Diapo 6 — Principe**

- `dist[source] = 0`, tous les autres à `INFINITY`.
- On extrait le nœud non traité de plus petite distance, on le fige, on relâche ses arêtes.
- Invariant : un nœud extrait a sa distance définitive (car les poids sont ≥ 0).
- Reconstruction du chemin avec `prev[]` en remontant depuis la cible.

**Diapo 7 — Deux versions**

| Version | Choix du minimum | Complexité |
|---|---|---|
| V1 | parcours du tableau | `O(V²)` |
| V2 | tas binaire, lazy deletion | `O((V + E) log V)` |

- Graphe routier creux : E ≈ 2 à 3 × V, donc V2 gagne largement quand V grandit.
- Lazy deletion : on ne met pas à jour une clé dans le tas, on réinsère et on ignore les entrées périmées au pop.
- Courbe V1 vs V2 en fonction de V : `À MESURER` (benchmark B1).

**Diapo 8 — Comment on sait que c'est juste**

- Graphe jouet de 5 nœuds (exemple du CLRS) avec distances connues (`data/toy/toy1.expected`).
- Oracle exhaustif : sur des graphes de 8 nœuds au plus, on énumère tous les chemins simples et on compare.
- Cas limites testés : nœud inatteignable (`INFINITY`), source invalide, chemin source → source.

Extrait de code possible (boucle principale de la V1, 10 lignes, `src/dijkstra.c`) : sélection du minimum puis relâchement.

## Texte oral

> Dijkstra, c'est l'idée suivante : on part de la source, et à chaque étape on prend le nœud le plus proche qu'on n'a pas encore traité. Comme les poids sont positifs, aucun autre chemin ne pourra être plus court plus tard, donc sa distance est définitive. On relâche ses voisins et on recommence.
>
> On l'a codé en deux temps. D'abord une version simple où on cherche le minimum en parcourant tout le tableau : c'est du V², facile à vérifier. Ensuite une version avec un tas binaire. Sur un graphe routier, chaque intersection a deux ou trois rues, donc E reste proche de V, et on passe à du (V + E) log V. Sur notre plus grand graphe, la différence est de `À MESURER`.
>
> Pour la correction, on compare à un oracle qui énumère tous les chemins sur des petits graphes.

## À préparer

- Animation ou suite de 3 captures : Dijkstra sur le graphe jouet, étape par étape.
- Courbe B1 : temps V1 vs V2 selon la taille du graphe.
- Extrait de code relu (pas plus de 10 lignes).

## À vérifier

- Le nom des fichiers : `djikstra` doit être renommé en `dijkstra` avant la soutenance (issue Phase 1).

## Transition

> Dijkstra explore dans toutes les directions, même à l'opposé de la cible. A\* corrige ça.
