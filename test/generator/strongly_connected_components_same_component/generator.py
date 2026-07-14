#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def reachability(
    vertex_count: int,
    edges: list[tuple[int, int]],
) -> list[list[bool]]:
    graph = [[] for _ in range(vertex_count)]
    for source, target in edges:
        graph[source].append(target)
    reachable = [[False] * vertex_count for _ in range(vertex_count)]
    for source in range(vertex_count):
        stack = [source]
        reachable[source][source] = True
        while stack:
            vertex = stack.pop()
            for target in graph[vertex]:
                if not reachable[source][target]:
                    reachable[source][target] = True
                    stack.append(target)
    return reachable


def write_case(
    out_dir: Path,
    index: int,
    vertex_count: int,
    edges: list[tuple[int, int]],
    queries: list[tuple[int, int]],
) -> None:
    reachable = reachability(vertex_count, edges)
    input_lines = [
        f"{vertex_count} {len(edges)} {len(queries)}",
        *(f"{source} {target}" for source, target in edges),
        *(f"{first} {second}" for first, second in queries),
    ]
    output_lines = []
    for first, second in queries:
        if not (0 <= first < vertex_count and 0 <= second < vertex_count):
            output_lines.append("ERR")
        else:
            output_lines.append(
                str(int(reachable[first][second] and reachable[second][first]))
            )
    stem = f"case_{index:02d}"
    (out_dir / f"{stem}.in").write_text(
        "\n".join(input_lines) + "\n", encoding="utf-8"
    )
    (out_dir / f"{stem}.out").write_text(
        "\n".join(output_lines) + "\n", encoding="utf-8"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    fixed_edges = [(0, 1), (1, 2), (2, 0), (2, 3), (3, 4), (4, 3)]
    cases = [
        (0, [], [(-1, 0), (0, 0)]),
        (
            5,
            fixed_edges,
            [(0, 0), (0, 2), (0, 3), (3, 4), (-1, 0), (0, 5)],
        ),
    ]
    rng = random.Random(20260714)
    for vertex_count in [1, 7, 20]:
        edges = [
            (source, target)
            for source in range(vertex_count)
            for target in range(vertex_count)
            if source != target and rng.randrange(7) == 0
        ]
        queries = [
            (rng.randrange(vertex_count), rng.randrange(vertex_count))
            for _ in range(80)
        ]
        queries.extend([(-1, 0), (0, vertex_count)])
        cases.append((vertex_count, edges, queries))

    for index, case in enumerate(cases):
        write_case(out_dir, index, *case)


if __name__ == "__main__":
    main()
