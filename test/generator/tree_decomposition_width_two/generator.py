#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def width_at_most_two(vertex_count: int, edges: list[tuple[int, int]]) -> bool:
    graph = [set() for _ in range(vertex_count)]
    for first, second in edges:
        graph[first].add(second)
        graph[second].add(first)
    remaining = vertex_count
    while remaining:
        candidate = next((v for v in range(vertex_count) if 0 <= len(graph[v]) <= 2 and v not in graph[v]), None)
        if candidate is None:
            return False
        neighbors = list(graph[candidate])
        for neighbor in neighbors:
            graph[neighbor].discard(candidate)
        graph[candidate].clear()
        graph[candidate].add(candidate)
        if len(neighbors) == 2:
            graph[neighbors[0]].add(neighbors[1])
            graph[neighbors[1]].add(neighbors[0])
        remaining -= 1
    return True


def write_case(
    out_dir: Path,
    index: int,
    vertex_count: int,
    edges: list[tuple[int, int]],
    expected: bool | None = None,
) -> None:
    if expected is None:
        expected = width_at_most_two(vertex_count, edges)
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
    write_case(args.out_dir, 1, 4, [(0,1),(0,2),(0,3),(1,2),(1,3),(2,3)], False)
    write_case(args.out_dir, 2, 6, [(i, (i + 1) % 6) for i in range(6)], True)

    rng = random.Random(2026071618)
    for case_index in range(3, 43):
        vertex_count = rng.randint(1, 10)
        edges = [
            (first, second)
            for first in range(vertex_count)
            for second in range(first + 1, vertex_count)
            if rng.randrange(3) == 0
        ]
        write_case(args.out_dir, case_index, vertex_count, edges)

    vertex_count = 100000
    write_case(
        args.out_dir,
        43,
        vertex_count,
        [(vertex, (vertex + 1) % vertex_count) for vertex in range(vertex_count)],
        True,
    )


if __name__ == "__main__":
    main()
