#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import itertools
import random
from pathlib import Path


def brute(
    size: int,
    edges: list[tuple[int, int, int]],
    terminals: list[int],
) -> int:
    best: int | None = None
    for mask in range(1 << len(edges)):
        parent = list(range(size))

        def leader(vertex: int) -> int:
            while parent[vertex] != vertex:
                parent[vertex] = parent[parent[vertex]]
                vertex = parent[vertex]
            return vertex

        cost = 0
        for index, (left, right, weight) in enumerate(edges):
            if not (mask >> index) & 1:
                continue
            left_root = leader(left)
            right_root = leader(right)
            if left_root != right_root:
                parent[right_root] = left_root
            cost += weight
        root = leader(terminals[0])
        if all(leader(vertex) == root for vertex in terminals):
            best = cost if best is None else min(best, cost)
    return -1 if best is None else best


def write_case(
    out_dir: Path,
    index: int,
    size: int,
    edges: list[tuple[int, int, int]],
    terminals: list[int],
) -> None:
    lines = [f"{size} {len(edges)} {len(terminals)}"]
    lines.append(" ".join(map(str, terminals)))
    lines.extend(f"{left} {right} {weight}" for left, right, weight in edges)
    (out_dir / f"case_{index:02d}.in").write_text(
        "\n".join(lines) + "\n", encoding="utf-8"
    )
    (out_dir / f"case_{index:02d}.out").write_text(
        f"{brute(size, edges, terminals)}\n", encoding="utf-8"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[tuple[int, list[tuple[int, int, int]], list[int]]] = [
        (1, [], [0]),
        (3, [(0, 1, 4)], [0, 2]),
        (5, [(0, 1, 2), (1, 2, 2), (1, 3, 1), (3, 4, 1), (2, 4, 10)], [0, 2, 4]),
    ]
    rng = random.Random(2026071312)
    for _ in range(35):
        size = rng.randrange(2, 9)
        pairs = list(itertools.combinations(range(size), 2))
        rng.shuffle(pairs)
        edge_count = rng.randrange(min(15, len(pairs)) + 1)
        edges = [
            (left, right, rng.randrange(1, 30))
            for left, right in pairs[:edge_count]
        ]
        terminal_count = rng.randrange(1, min(5, size) + 1)
        terminals = sorted(rng.sample(range(size), terminal_count))
        cases.append((size, edges, terminals))

    for index, (size, edges, terminals) in enumerate(cases):
        write_case(args.out_dir, index, size, edges, terminals)


if __name__ == "__main__":
    main()
