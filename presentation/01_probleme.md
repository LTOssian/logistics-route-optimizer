# 1. Problème et modélisation

Orateur : à définir — 1 min 30 · Vocabulaire : [[glossaire]]

## Message clé

Planifier des livraisons revient à enchaîner quatre problèmes de graphes classiques, chacun construit sur le précédent.

## Diapos

**Diapo 1 — Titre**

- Optimisation de tournées logistiques
- Noms du groupe, cours, date

**Diapo 2 — Le problème**

- Un entrepôt, N clients, chaque client a une demande (nombre de colis).
- Plusieurs camionnettes de capacité Q.
- Trois questions : quel chemin entre deux adresses ? dans quel ordre livrer ? quel client dans quel camion ?

**Diapo 3 — Modélisation**

- Réseau routier = graphe G = (V, E), poids = longueur de la rue en mètres.
- Plus court chemin entre deux points : Dijkstra, A\*.
- Matrice des distances entre le dépôt et les N clients.
- Une seule tournée (TSP), puis plusieurs véhicules à capacité limitée (CVRP).
- Schéma du pipeline (voir README).

## Texte oral

> Quand un livreur part le matin, il faut répondre à trois questions : par où passer entre deux adresses, dans quel ordre faire les livraisons, et si on a plusieurs camions, qui livre qui. On a modélisé la ville comme un graphe : les intersections sont les nœuds, les rues sont les arêtes, avec leur longueur en mètres. Ensuite on a quatre étages. Le plus court chemin entre deux points, puis une matrice des distances entre tous les clients, puis la tournée d'un seul véhicule, le TSP, et enfin la répartition sur plusieurs véhicules, le CVRP. Le TSP et le CVRP sont NP-difficiles, donc on utilise des heuristiques et on mesure à quelle distance on est de l'optimum.

## À préparer

- Schéma du pipeline en 5 boîtes (Excalidraw ou dessin dans les diapos).
- Une carte de la zone de Lyon avec le dépôt et quelques clients (capture générée par `scripts/`).

## Transition

> Avant de calculer quoi que ce soit, il faut un graphe. Je vous montre d'où il vient.
