# 7. Démo live

Orateur : à définir — 1 min 45 · Vocabulaire : [[glossaire]]

## Message clé

L'outil fonctionne de bout en bout sur la vraie carte de Lyon, en quelques millisecondes.

## Diapo

**Diapo 13 — Démo** : une diapo de transition, puis on passe au terminal. Compter 15 secondes pour basculer.

## Script (à répéter tel quel)

Terminal ouvert à la racine du dépôt, police grande. Tout passe par Docker : le binaire est un exécutable Linux, et la même image garantit les mêmes points tirés avec la graine 42 que dans les benchmarks. Lancer `docker compose run --rm dev make` avant la soutenance.

```sh
# 1. Plus court chemin : Dijkstra puis A*, même distance, moins de nœuds explorés
docker compose run --rm dev ./build/lro path data/osm/lyon_2000 <source> <cible> --algo dijkstra
docker compose run --rm dev ./build/lro path data/osm/lyon_2000 <source> <cible> --algo astar

# 2. Tournée d'un véhicule : NN puis 2-opt
docker compose run --rm dev ./build/lro tsp data/osm/lyon_2000 --points 20 --seed 42

# 3. Plusieurs véhicules
docker compose run --rm dev ./build/lro cvrp data/osm/lyon_2000 --points 30 --capacity 60 --seed 42
```

Ces commandes vont dans `demo.sh`. Choisir `<source>` et `<cible>` à l'avance, éloignés l'un de l'autre, pour que l'écart Dijkstra / A\* soit visible. Les noter ici : `À CHOISIR`.

Sortie attendue de chaque commande : distance totale, temps, nœuds explorés (path), nombre de véhicules (cvrp). Si on a le temps, ouvrir l'image de la tournée générée par `scripts/plot_route.py`.

## Ce qu'on dit pendant la démo

> Je lance Dijkstra entre ces deux points : `X` mètres, `Y` nœuds explorés. Même requête avec A\* : même distance, mais `Z` nœuds seulement. Maintenant 20 livraisons : le plus proche voisin donne `A` mètres, après 2-opt `B`. Et avec des camions de capacité 60 : `K` tournées.

## Plan B

- Une vidéo de la démo (enregistrement d'écran) sur le poste et sur une clé USB.
- Des captures des sorties dans les diapos annexes.
- Si la démo plante, on passe directement à la vidéo sans chercher à corriger en direct.

## Checklist le jour J

- [ ] Données `data/osm/lyon_2000` présentes (versionnées, jamais régénérées après le gel du code).
- [ ] `docker compose run --rm dev sh -c 'make clean && make'` sans warning.
- [ ] Commandes dans l'historique du terminal ou dans un fichier `demo.sh`.
- [ ] Vidéo du plan B testée.
- [ ] Pas besoin d'internet pendant la démo : l'image Docker est déjà construite.
