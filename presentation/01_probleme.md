# 1. Problème et modélisation

Orateur : à définir — 1 min 15 · Vocabulaire : [[glossaire]]

## Message clé

Planifier des livraisons revient à enchaîner quatre problèmes de graphes classiques, chacun construit sur le précédent.

## Diapos

**Diapo 1 — Titre**

- Optimisation de tournées logistiques
- Noms complets des trois membres, nom du cours, date

**Diapo 2 — Le problème et sa modélisation**

- Un entrepôt, N clients avec une demande, des camionnettes de capacité Q.
- Trois questions : quel chemin entre deux adresses ? dans quel ordre livrer ? quel client dans quel camion ?
- Schéma : OpenStreetMap → graphe → plus court chemin → matrice → TSP → CVRP.
- Graphe G = (V, E) : intersections et rues, poids = longueur en mètres.

## Texte oral

> Quand un livreur part le matin, il faut répondre à trois questions : par où passer entre deux adresses, dans quel ordre faire les livraisons, et si on a plusieurs camions, qui livre qui. On a modélisé la ville comme un graphe : les intersections sont les nœuds, les rues sont les arêtes, avec leur longueur en mètres. Ensuite on a quatre étages. Le plus court chemin entre deux points, puis une matrice des distances entre tous les clients, puis la tournée d'un seul véhicule, le TSP, et enfin la répartition sur plusieurs véhicules, le CVRP. Le TSP et le CVRP sont NP-difficiles, donc on utilise des heuristiques et on mesure à quelle distance on est de l'optimum.

## À préparer

- Schéma du pipeline en 5 boîtes (Excalidraw ou dessin dans les diapos).
- Une carte de la zone de Lyon avec le dépôt et quelques clients (capture générée par `scripts/`).

## Transition

> Avant de calculer quoi que ce soit, il faut un graphe. Je vous montre d'où il vient.
