#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


Edge = tuple[int, int, int]


def disjoint_cliques(group_size: int, group_count: int) -> tuple[int, list[Edge]]:
    n = group_size * group_count
    edges: list[Edge] = []
    for group in range(group_count):
        first = group * group_size
        for left in range(first, first + group_size):
            for right in range(left + 1, first + group_size):
                edges.append((left, right, 5 + (left + right) % 3))
                if (left + right) % 4 == 0:
                    edges.append((right, left, 1))
        edges.append((first, first, 10**9))
    return n, edges


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(2026071302)

    cases: list[tuple[int, list[Edge], int]] = [(0, [], 0), (1, [(0, 0, 999)], 1)]
    for group_size, group_count in [(3, 2), (4, 2), (3, 3), (6, 1)]:
        n, edges = disjoint_cliques(group_size, group_count)
        cases.append((n, edges, group_count))
    for _ in range(25):
        n = rng.randrange(2, 45)
        edges: list[Edge] = []
        for _ in range(rng.randrange(0, n * 6 + 1)):
            left = rng.randrange(n)
            right = rng.randrange(n)
            weight = rng.randrange(0, 10**7)
            edges.append((left, right, weight))
            if rng.randrange(8) == 0:
                edges.append((right, left, weight // 3))
        cases.append((n, edges, 0))

    input_lines = [str(len(cases))]
    output_lines = []
    for identifier, (n, edges, planted_groups) in enumerate(cases):
        input_lines.append(f"{identifier} {n} {len(edges)} {planted_groups}")
        input_lines.extend(f"{left} {right} {weight}" for left, right, weight in edges)
        output_lines.append(f"OK {identifier}")
    (out_dir / "case_00.in").write_text("\n".join(input_lines) + "\n", encoding="utf-8")
    (out_dir / "case_00.out").write_text("\n".join(output_lines) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
