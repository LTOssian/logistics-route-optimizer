"""Exporte le réseau routier de Lyon en CSV pour le programme C.

Pour chaque demi-côté R, produit data/osm/lyon_<R>/ :
  edges.csv  "n,m" puis "u,v,weight" (mètres), chaque rue dans les deux sens
  nodes.csv  "id,lat,lon", une ligne par nœud de 0 à n-1
  meta.txt   date d'extraction, version d'OSMnx, paramètres, tailles

Usage (depuis la racine du dépôt) :
  docker compose run --rm dev python scripts/osm_to_csv.py
  docker compose run --rm dev python scripts/osm_to_csv.py --radius 500
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


def haversine_m(lat1, lon1, lat2, lon2):
    """Même formule que haversine_m() en C."""
    phi1 = math.radians(lat1)
    phi2 = math.radians(lat2)
    dphi = math.radians(lat2 - lat1)
    dlmb = math.radians(lon2 - lon1)
    a = math.sin(dphi / 2) ** 2 + math.cos(phi1) * math.cos(phi2) * math.sin(dlmb / 2) ** 2
    return 2 * EARTH_RADIUS_M * math.asin(math.sqrt(min(a, 1.0)))


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
    - Un poids plus petit que le vol d'oiseau rendrait Haversine non admissible :
      on le remonte au vol d'oiseau (compté dans meta.txt). En pratique l'écart
      est de l'ordre de 1e-10 m : un arrondi entre le calcul d'OSMnx et le nôtre.
    """
    osm_ids = sorted(graph.nodes)  # ordre stable d'une extraction à l'autre
    index = {osm_id: i for i, osm_id in enumerate(osm_ids)}
    coords = [(graph.nodes[o]["y"], graph.nodes[o]["x"]) for o in osm_ids]

    best = {}
    self_loops = 0
    for u, v, data in graph.edges(data=True):
        a, b = index[u], index[v]
        if a == b:
            self_loops += 1
            continue
        key = (min(a, b), max(a, b))
        length = float(data["length"])
        if key not in best or length < best[key]:
            best[key] = length

    raised = 0
    max_raise = 0.0
    for (a, b), length in best.items():
        straight = haversine_m(*coords[a], *coords[b])
        if length < straight:
            best[(a, b)] = straight
            raised += 1
            max_raise = max(max_raise, straight - length)

    stats = {
        "osm_edges": graph.number_of_edges(),
        "self_loops_removed": self_loops,
        "parallel_merged": graph.number_of_edges() - self_loops - len(best),
        "weights_raised_to_haversine": raised,
        "max_raise_m": max_raise,
    }
    return coords, best, stats


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
        "extracted_at": datetime.now(timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ"),
        "osmnx_version": ox.__version__,
        "center_lat": CENTER[0],
        "center_lon": CENTER[1],
        "half_side_m": radius,
        "zone": "carre (dist_type=bbox)",
        "network_type": "drive",
        "nodes": n,
        "edges_lines": m,
        "streets": len(streets),
        **stats,
    }
    with open(os.path.join(out_dir, "meta.txt"), "w", newline="\n") as f:
        for key, value in meta.items():
            f.write(f"{key}={value}\n")
    return n, m


def check(out_dir, n, m, streets):
    """Relit les fichiers et vérifie ce que le C attend."""
    with open(os.path.join(out_dir, "nodes.csv")) as f:
        rows = f.read().splitlines()[1:]
    assert len(rows) == n, "nodes.csv : nombre de lignes != n"
    assert all(r.split(",")[0] == str(i) for i, r in enumerate(rows)), "ids non denses"

    with open(os.path.join(out_dir, "edges.csv")) as f:
        lines = f.read().splitlines()
    assert lines[1] == f"{n},{m}" and len(lines) - 3 == m, "edges.csv : en-tête != contenu"

    check_graph = nx.Graph()
    check_graph.add_nodes_from(range(n))
    check_graph.add_edges_from(streets)
    assert nx.is_connected(check_graph), "graphe non connexe"


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--radius", type=int, nargs="+", default=list(RADII))
    parser.add_argument("--out", default="data/osm")
    parser.add_argument("--timeout", type=int, default=300)
    parser.add_argument("--overpass-url", help="miroir Overpass si le serveur principal sature")
    args = parser.parse_args()

    ox.settings.cache_folder = "data/osm/raw/cache"
    ox.settings.use_cache = True
    ox.settings.requests_timeout = args.timeout
    if args.overpass_url:
        ox.settings.overpass_url = args.overpass_url

    for radius in args.radius:
        out_dir = os.path.join(args.out, f"lyon_{radius}")
        print(f"R = {radius} m : téléchargement...", flush=True)
        graph = download(radius)
        coords, streets, stats = build_tables(graph)
        n, m = write_files(out_dir, radius, coords, streets, stats)
        check(out_dir, n, m, streets)
        print(f"  {out_dir} : {n} nœuds, {len(streets)} rues ({m} arêtes orientées), {stats}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
