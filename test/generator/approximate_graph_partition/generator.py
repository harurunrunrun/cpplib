#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


Edge = tuple[int, int, int]


def planted_cliques(group_size: int, groups: int) -> tuple[int, list[Edge]]:
    n = group_size * groups
    edges: list[Edge] = []
    for group in range(groups):
        first = group * group_size
        for left in range(first, first + group_size):
            for right in range(left + 1, first + group_size):
                edges.append((left, right, 10))
    for group in range(groups - 1):
        edges.append(((group + 1) * group_size - 1, (group + 1) * group_size, 1))
    return n, edges


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(2026071301)

    cases: list[tuple[int, list[Edge], int, int]] = [
        (0, [], 2, 0),
        (1, [(0, 0, 999)], 2, 0),
        (2, [(0, 1, 7)], 2, 7),
        (4, [(0, 1, 3), (1, 0, 4), (0, 0, 999), (2, 3, 8)], 2, 0),
    ]
    n, edges = planted_cliques(4, 2)
    cases.append((n, edges, 2, 1))
    n, edges = planted_cliques(5, 2)
    cases.append((n, edges, 2, 1))
    for _ in range(24):
        n = rng.randrange(2, 34)
        edge_count = rng.randrange(0, n * 5 + 1)
        edges = []
        for _ in range(edge_count):
            left = rng.randrange(n)
            right = rng.randrange(n)
            weight = rng.randrange(0, 10**6)
            edges.append((left, right, weight))
            if rng.randrange(7) == 0:
                edges.append((right, left, weight // 2))
        cases.append((n, edges, 2, -1))

    input_lines = [str(len(cases))]
    output_lines = []
    for identifier, (n, edges, maximum_imbalance, upper_bound) in enumerate(cases):
        input_lines.append(f"{identifier} {n} {len(edges)} {maximum_imbalance} {upper_bound}")
        input_lines.extend(f"{left} {right} {weight}" for left, right, weight in edges)
        output_lines.append(f"OK {identifier}")
    (out_dir / "case_00.in").write_text("\n".join(input_lines) + "\n", encoding="utf-8")
    (out_dir / "case_00.out").write_text("\n".join(output_lines) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
