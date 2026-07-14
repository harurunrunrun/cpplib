#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def write_case(
    out_dir: Path,
    index: int,
    names: list[str],
    edges: list[tuple[int, int, int]],
    queries: list[tuple[int, int]],
) -> None:
    vertex_count = len(names)
    infinity = 10**30
    distance = [[infinity] * vertex_count for _ in range(vertex_count)]
    for vertex in range(vertex_count):
        distance[vertex][vertex] = 0
    for source, target, cost in edges:
        distance[source][target] = min(distance[source][target], cost)
    for middle in range(vertex_count):
        for source in range(vertex_count):
            for target in range(vertex_count):
                distance[source][target] = min(
                    distance[source][target],
                    distance[source][middle] + distance[middle][target],
                )

    name = f"case_{index:03d}"
    input_lines = [
        f"{vertex_count} {len(edges)} {len(queries)}",
        " ".join(names),
        *(f"{source} {target} {cost}" for source, target, cost in edges),
        *(f"{names[source]} {names[target]}" for source, target in queries),
    ]
    output_lines = [
        "unreachable" if distance[source][target] == infinity else str(distance[source][target])
        for source, target in queries
    ]
    (out_dir / f"{name}.in").write_text("\n".join(input_lines) + "\n", encoding="ascii")
    (out_dir / f"{name}.out").write_text("\n".join(output_lines) + "\n", encoding="ascii")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    write_case(out_dir, 0, ["a"], [], [(0, 0)])
    write_case(out_dir, 1, ["a", "b", "c"], [(0, 1, 4), (1, 2, 3)], [(0, 2), (2, 0), (0, 2)])

    rng = random.Random(100000107)
    for index in range(2, 102):
        vertex_count = rng.randint(1, 10)
        names = [f"v{vertex}" for vertex in range(vertex_count)]
        edges = [
            (source, target, rng.randint(0, 100))
            for source in range(vertex_count)
            for target in range(vertex_count)
            if rng.randrange(4) == 0
        ]
        queries = [
            (rng.randrange(vertex_count), rng.randrange(vertex_count))
            for _ in range(rng.randint(1, 30))
        ]
        write_case(out_dir, index, names, edges, queries)


if __name__ == "__main__":
    main()
