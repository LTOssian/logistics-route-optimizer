# 00 — Contexte du projet

## Intitulé

**Optimisation de tournées logistiques**

## Objectif

Implémenter et comparer des algorithmes de graphes appliqués à la logistique : calcul d'itinéraires et planification de tournées de livraison. Ces problèmes sont au cœur des GPS, des services de livraison et de la gestion de flottes de véhicules.

On travaille sur un **réseau routier réel extrait d'OpenStreetMap** (une ville ou une région).

## Fonctionnalités attendues

Le système doit permettre de :

1. **Calculer le plus court chemin** entre deux points.
2. **Calculer une matrice de distances** entre un ensemble de points de livraison.
3. **Construire une tournée** passant par tous les points (problème du voyageur de commerce, TSP).
4. **Répartir les livraisons** entre plusieurs véhicules de capacité limitée (problème de tournées de véhicules, VRP).

## Minimum à implémenter

- **Dijkstra** et **A\*** pour le plus court chemin.
- Une **heuristique constructive** : plus proche voisin ou **Clarke & Wright**.
- Une **méthode d'amélioration locale** : **2-opt** ou **Or-opt**.

## Bonus (motivés)

- **Contraction Hierarchies** (CH).
- Ou une **métaheuristique** : recuit simulé, recherche tabou, algorithme génétique.

## Comparaison attendue

- Faire varier la **taille du graphe** et le **nombre de points de livraison**.
- Mesurer :
  - le **temps de calcul** ;
  - la **qualité des solutions** (distance totale, écart à la meilleure solution connue).
- Indiquer les **complexités** des algorithmes.
- Discuter du **compromis qualité de la solution / temps de calcul**.

## Article scientifique

Lire **au moins un article** de la liste ci-dessous et en présenter les idées principales dans le rapport, en les reliant aux choix et aux résultats.

## Références

- **Dijkstra (1959)**, *A Note on Two Problems in Connexion with Graphs* — l'article original, trois pages.
- **Hart, Nilsson & Raphael (1968)**, *A Formal Basis for the Heuristic Determination of Minimum Cost Paths* — l'article de A\*.
- **Clarke & Wright (1964)**, *Scheduling of Vehicles from a Central Depot to a Number of Delivery Points* — l'heuristique des économies, toujours utilisée.
- **Geisberger et al. (2008)**, *Contraction Hierarchies: Faster and Simpler Hierarchical Routing in Road Networks* — la technique derrière les GPS modernes.
- **Bast et al. (2016)**, *Route Planning in Transportation Networks* — une synthèse très complète des techniques actuelles.

## Contraintes d'ingénierie

- **Langage des algorithmes : C** (les scripts de données peuvent être en Python).
- Compilation séparée, `Makefile`.
- Compiler avec `-Wall -Wextra -Werror` et lire les messages d'erreur.
- Un **commit à la fin de chaque unité de travail** (pas un seul commit à la fin).
- Tests unitaires pour les modules critiques.
- Reproductibilité : graines fixes, versions et dates documentées.

## Livrables

- Dépôt Git avec code, tests, scripts, données (ou script de génération) et rapport.
- Rapport académique structuré (voir `../PLAN.md`, phase 8).
