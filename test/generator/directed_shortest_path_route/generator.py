#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def shortest_distance(
    vertex_count: int,
    edges: list[tuple[int, int, int]],
    source: int,
    target: int,
) -> int | None:
    distance: list[int | None] = [None] * vertex_count
    distance[source] = 0
    for _ in range(vertex_count - 1):
        changed = False
        for from_vertex, to_vertex, cost in edges:
            if distance[from_vertex] is None:
                continue
            candidate = distance[from_vertex] + cost
            if distance[to_vertex] is None or candidate < distance[to_vertex]:
                distance[to_vertex] = candidate
                changed = True
        if not changed:
            break
    return distance[target]


def write_case(
    out_dir: Path,
    index: int,
    vertex_count: int,
    edges: list[tuple[int, int, int]],
    source: int,
    target: int,
) -> None:
    name = f"case_{index:03d}"
    lines = [f"{vertex_count} {len(edges)} {source} {target}"]
    lines.extend(f"{from_vertex} {to_vertex} {cost}" for from_vertex, to_vertex, cost in edges)
    (out_dir / f"{name}.in").write_text("\n".join(lines) + "\n", encoding="ascii")
    distance = shortest_distance(vertex_count, edges, source, target)
    (out_dir / f"{name}.out").write_text(
        "-1\n" if distance is None else f"{distance}\n", encoding="ascii"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    fixed = [
        (1, [], 0, 0),
        (4, [(0, 1, 5), (1, 2, 6)], 0, 3),
        (3, [(0, 1, 5), (1, 2, 6), (0, 2, 20)], 0, 2),
        (3, [(0, 1, 8), (0, 1, 2), (1, 2, 0)], 0, 2),
    ]
    for index, (vertex_count, edges, source, target) in enumerate(fixed):
        write_case(out_dir, index, vertex_count, edges, source, target)

    rng = random.Random(928741)
    for index in range(len(fixed), 65):
        vertex_count = rng.randint(1, 12)
        edge_count = rng.randint(0, 40)
        edges = [
            (
                rng.randrange(vertex_count),
                rng.randrange(vertex_count),
                rng.randint(0, 50),
            )
            for _ in range(edge_count)
        ]
        write_case(
            out_dir,
            index,
            vertex_count,
            edges,
            rng.randrange(vertex_count),
            rng.randrange(vertex_count),
        )


if __name__ == "__main__":
    main()
