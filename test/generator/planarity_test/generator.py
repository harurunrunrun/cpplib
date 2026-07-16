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
    edges: list[tuple[int, int]],
    expected: bool,
) -> None:
    lines = [f"{vertex_count} {len(edges)}"]
    lines.extend(f"{first} {second}" for first, second in edges)
    (out_dir / f"case_{index:03d}.in").write_text(
        "\n".join(lines) + "\n", encoding="utf-8"
    )
    (out_dir / f"case_{index:03d}.out").write_text(
        f"{int(expected)}\n", encoding="utf-8"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)

    write_case(args.out_dir, 0, 0, [], True)
    write_case(args.out_dir, 1, 5, [(i, j) for i in range(5) for j in range(i + 1, 5)], False)
    write_case(args.out_dir, 2, 6, [(i, j) for i in range(3) for j in range(3, 6)], False)

    rng = random.Random(2026071619)
    for case_index in range(3, 33):
        vertex_count = rng.randint(20, 200)
        edges: set[tuple[int, int]] = set()
        for vertex in range(1, vertex_count):
            parent = rng.randrange(vertex)
            edges.add((parent, vertex))
            if vertex >= 2 and rng.randrange(2) == 0:
                first = rng.randrange(vertex)
                second = rng.randrange(vertex)
                if first != second:
                    edges.add((min(first, second), max(first, second)))
        # A subgraph of a graph formed by repeatedly attaching a vertex to an
        # existing edge is planar; the generator instead keeps a sparse tree
        # plus noncrossing fan edges from vertex 0.
        edges = {(v - 1, v) for v in range(1, vertex_count)}
        edges.update((0, v) for v in range(2, vertex_count))
        for edge in list(edges):
            if rng.randrange(5) == 0 and edge[0] + 1 != edge[1]:
                edges.remove(edge)
        write_case(args.out_dir, case_index, vertex_count, sorted(edges), True)

    for case_index in range(33, 43):
        left = 3 + case_index
        right = 3 + case_index // 2
        edges = [(first, left + second) for first in range(left) for second in range(right)]
        write_case(args.out_dir, case_index, left + right, edges, False)

    right = 1000
    k3_many = [
        (first, 3 + second) for first in range(3) for second in range(right)
    ]
    write_case(args.out_dir, 43, 3 + right, k3_many, False)
    rows = 300
    columns = 300
    grid = []
    for row in range(rows):
        for column in range(columns):
            vertex = row * columns + column
            if row + 1 < rows:
                grid.append((vertex, vertex + columns))
            if column + 1 < columns:
                grid.append((vertex, vertex + 1))
    write_case(args.out_dir, 44, rows * columns, grid, True)


if __name__ == "__main__":
    main()
