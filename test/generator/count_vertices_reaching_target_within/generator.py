#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def write_case(
    out_dir: Path,
    index: int,
    vertex_count: int,
    edges: list[tuple[int, int, int]],
    target: int,
    limit: int,
) -> None:
    infinity = 10**30
    distance = [[infinity] * vertex_count for _ in range(vertex_count)]
    for vertex in range(vertex_count):
        distance[vertex][vertex] = 0
    for source, destination, cost in edges:
        distance[source][destination] = min(
            distance[source][destination], cost
        )
    for middle in range(vertex_count):
        for source in range(vertex_count):
            for destination in range(vertex_count):
                distance[source][destination] = min(
                    distance[source][destination],
                    distance[source][middle] + distance[middle][destination],
                )

    answer = sum(
        distance[source][target] <= limit
        for source in range(vertex_count)
    )
    name = f"case_{index:03d}"
    input_lines = [
        f"{vertex_count} {len(edges)} {target} {limit}",
        *(f"{source} {destination} {cost}" for source, destination, cost in edges),
    ]
    (out_dir / f"{name}.in").write_text(
        "\n".join(input_lines) + "\n", encoding="ascii"
    )
    (out_dir / f"{name}.out").write_text(f"{answer}\n", encoding="ascii")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    write_case(out_dir, 0, 1, [], 0, 0)
    write_case(out_dir, 1, 4, [(0, 1, 2), (1, 2, 3), (3, 2, 6)], 2, 5)

    rng = random.Random(249407)
    for index in range(2, 52):
        vertex_count = rng.randint(1, 12)
        edges = [
            (source, destination, rng.randint(0, 80))
            for source in range(vertex_count)
            for destination in range(vertex_count)
            if rng.randrange(4) == 0
        ]
        write_case(
            out_dir,
            index,
            vertex_count,
            edges,
            rng.randrange(vertex_count),
            rng.randint(-1, 120),
        )


if __name__ == "__main__":
    main()
