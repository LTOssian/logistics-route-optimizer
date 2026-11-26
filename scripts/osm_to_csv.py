"""Exporte le réseau routier de Lyon en CSV pour le programme C.

Pour chaque demi-côté R, produit data/osm/lyon_<R>/ :
  edges.csv  "n,m" puis "u,v,weight" (mètres), chaque rue dans les deux sens
  nodes.csv  "id,lat,lon", une ligne par nœud de 0 à n-1
  meta.txt   heure d'exécution, version d'OSMnx, paramètres, tailles

Les jeux committés sont gelés : le script refuse d'écraser un dossier existant
sans --force. Sans le cache local, Overpass renvoie la carte OSM du jour, donc
une régénération peut donner d'autres nœuds et une autre numérotation.

Usage (depuis la racine du dépôt) :
  docker compose run --rm dev python scripts/osm_to_csv.py --out /tmp/osm
  docker compose run --rm dev python scripts/osm_to_csv.py --radius 500 --force
"""

import argparse
import math
import os
import sys
from datetime import datetime, timezone

import networkx as nx
import osmnx as ox

CENTER = (45.7578, 4.8320)  # place Bellecour (lat, lon)
RADII = (500, 1000, 2000, 4000)
EARTH_RADIUS_M = 6371009.0  # même rayon qu'OSMnx et que src/astar.h

# Marge ajoutée au vol d'oiseau quand on remonte un poids. Le C et Python
# n'arrondissent pas exactement pareil (écart observé : 3e-14 m) ; 1e-9 m
# garantit poids >= Haversine quel que soit le langage qui le calcule.
MARGIN_M = 1e-9
# Au-delà, ce n'est plus un arrondi mais une donnée OSM incohérente : on s'arrête.
MAX_RAISE_M = 1e-6


class ExportError(Exception):
    pass


def haversine_m(lat1, lon1, lat2, lon2):
    """Même formule, dans le même ordre d'opérations, que haversine_m() en C."""
    phi1 = lat1 * math.pi / 180.0
    phi2 = lat2 * math.pi / 180.0
    dphi = (lat2 - lat1) * math.pi / 180.0
    dlmb = (lon2 - lon1) * math.pi / 180.0
    s_phi = math.sin(dphi / 2.0)
    s_lmb = math.sin(dlmb / 2.0)
    a = s_phi * s_phi + math.cos(phi1) * math.cos(phi2) * s_lmb * s_lmb
    return 2.0 * EARTH_RADIUS_M * math.asin(math.sqrt(min(a, 1.0)))


def download(radius):
    """Télécharge le réseau « drive » dans un carré de demi-côté `radius`.

    graph_from_point découpe un carré (dist_type="bbox" par défaut) et ne garde
    que la plus grande composante connexe (retain_all=False par défaut).
    """
    graph = ox.graph_from_point(CENTER, dist=radius, network_type="drive")
    return ox.convert.to_undirected(graph)


def build_tables(graph):
    """Renumérote les nœuds de 0 à n-1 et regroupe les arêtes par paire.

    - Les boucles (u == v) sont retirées : elles ne raccourcissent aucun chemin.
    - Entre deux carrefours reliés par plusieurs rues, on garde la plus courte :
      les plus courts chemins ne changent pas.
    - Un poids sous le vol d'oiseau (+ MARGIN_M) rendrait Haversine non
      admissible : on le remonte. En pratique l'écart est d'environ 1e-10 m,
      un arrondi entre le calcul d'OSMnx et le nôtre.
    """
    osm_ids = sorted(graph.nodes)  # numérotation indépendante de l'ordre de parcours
    index = {osm_id: i for i, osm_id in enumerate(osm_ids)}
    coords = [(graph.nodes[o]["y"], graph.nodes[o]["x"]) for o in osm_ids]

    streets = {}
    self_loops = 0
    for u, v, data in graph.edges(data=True):
        a, b = index[u], index[v]
        if a == b:
            self_loops += 1
            continue
        key = (min(a, b), max(a, b))
        length = float(data["length"])
        if key not in streets or length < streets[key]:
            streets[key] = length

    raised = 0
    max_raise = 0.0
    for (a, b), length in streets.items():
        floor = haversine_m(*coords[a], *coords[b]) + MARGIN_M
        if length < floor:
            if floor - length > MAX_RAISE_M:
                raise ExportError(
                    f"rue {a}-{b} : {length} m, plus court que le vol d'oiseau "
                    f"({floor} m) : donnée OSM incohérente")
            streets[(a, b)] = floor
            raised += 1
            max_raise = max(max_raise, floor - length)

    stats = {
        "osm_edges": graph.number_of_edges(),
        "self_loops_removed": self_loops,
        "parallel_merged": graph.number_of_edges() - self_loops - len(streets),
        "weights_raised": raised,
        "max_raise_m": max_raise,
    }
    return coords, streets, stats


def write_files(out_dir, radius, coords, streets, stats):
    os.makedirs(out_dir, exist_ok=True)
    n = len(coords)
    m = 2 * len(streets)

    with open(os.path.join(out_dir, "nodes.csv"), "w", newline="\n") as f:
        f.write("id,lat,lon\n")
        for i, (lat, lon) in enumerate(coords):
            f.write(f"{i},{lat!r},{lon!r}\n")

    with open(os.path.join(out_dir, "edges.csv"), "w", newline="\n") as f:
        f.write(f"n,m\n{n},{m}\nu,v,weight\n")
        for (a, b), w in sorted(streets.items()):
            f.write(f"{a},{b},{w!r}\n{b},{a},{w!r}\n")

    meta = {
        # heure d'exécution du script ; avec le cache, les données peuvent être plus anciennes
        "run_at": datetime.now(timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ"),
        "osmnx_version": ox.__version__,
        "center_lat": CENTER[0],
        "center_lon": CENTER[1],
        "R": radius,
        "zone": "carre de demi-cote R (dist_type=bbox)",
        "network_type": "drive",
        "overpass_url": ox.settings.overpass_url,
        "oriented": "non (chaque rue dans les deux sens, sens uniques ignores)",
        "n": n,
        "m": m,
        "streets": len(streets),
        **stats,
    }
    with open(os.path.join(out_dir, "meta.txt"), "w", newline="\n") as f:
        for key, value in meta.items():
            f.write(f"{key}={value}\n")
    return n, m


def check(out_dir, n, m):
    """Relit les fichiers écrits et vérifie ce que le C attend."""
    with open(os.path.join(out_dir, "nodes.csv")) as f:
        rows = f.read().splitlines()[1:]
    if len(rows) != n:
        raise ExportError("nodes.csv : nombre de lignes != n")
    if any(r.split(",")[0] != str(i) for i, r in enumerate(rows)):
        raise ExportError("nodes.csv : ids non denses ou dans le désordre")

    with open(os.path.join(out_dir, "edges.csv")) as f:
        lines = f.read().splitlines()
    if lines[1] != f"{n},{m}" or len(lines) - 3 != m:
        raise ExportError("edges.csv : en-tête différent du contenu")

    reread = nx.Graph()
    reread.add_nodes_from(range(n))
    for line in lines[3:]:
        u, v, w = line.split(",")
        reread.add_edge(int(u), int(v))
    if not nx.is_connected(reread):
        raise ExportError("edges.csv : graphe non connexe")


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--radius", type=int, nargs="+", default=list(RADII),
                        help="demi-côté R du carré, en mètres (défaut : 500 1000 2000 4000)")
    parser.add_argument("--out", default="data/osm")
    parser.add_argument("--force", action="store_true",
                        help="écraser un dossier lyon_<R> existant (données gelées)")
    parser.add_argument("--timeout", type=int, default=300)
    parser.add_argument("--overpass-url", help="miroir Overpass si le serveur principal sature")
    args = parser.parse_args()

    existing = [r for r in args.radius
                if os.path.exists(os.path.join(args.out, f"lyon_{r}"))]
    if existing and not args.force:
        print(f"refus : lyon_{existing} existe déjà dans {args.out}. Ces données sont "
              "gelées ; utiliser --out <autre dossier>, ou --force pour les écraser.",
              file=sys.stderr)
        return 1

    ox.settings.cache_folder = "data/osm/raw/cache"
    ox.settings.use_cache = True
    ox.settings.requests_timeout = args.timeout
    if args.overpass_url:
        ox.settings.overpass_url = args.overpass_url

    try:
        # Tout télécharger avant d'écrire : si Overpass plante au milieu, aucun
        # fichier n'est modifié, et les 4 jeux viennent toujours d'une même extraction.
        tables = {}
        for radius in args.radius:
            print(f"R = {radius} m : téléchargement...", flush=True)
            tables[radius] = build_tables(download(radius))

        for radius, (coords, streets, stats) in tables.items():
            out_dir = os.path.join(args.out, f"lyon_{radius}")
            n, m = write_files(out_dir, radius, coords, streets, stats)
            check(out_dir, n, m)
            print(f"  {out_dir} : {n} nœuds, {len(streets)} rues ({m} arêtes orientées), {stats}")
    except ExportError as error:
        print(f"erreur : {error}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
