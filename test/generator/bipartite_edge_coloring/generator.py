#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def delta(
    left_size: int,
    right_size: int,
    edges: list[tuple[int, int]],
) -> int:
    left_degree = [0] * left_size
    right_degree = [0] * right_size
    for left, right in edges:
        left_degree[left] += 1
        right_degree[right] += 1
    return max([0, *left_degree, *right_degree])


def write_case(
    out_dir: Path,
    index: int,
    left_size: int,
    right_size: int,
    edges: list[tuple[int, int]],
) -> None:
    input_lines = [f"{left_size} {right_size} {len(edges)}"]
    input_lines.extend(f"{left} {right}" for left, right in edges)
    (out_dir / f"case_{index:02d}.in").write_text(
        "\n".join(input_lines) + "\n", encoding="utf-8"
    )
    (out_dir / f"case_{index:02d}.out").write_text(
        f"{delta(left_size, right_size, edges)} 1\n", encoding="utf-8"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[tuple[int, int, list[tuple[int, int]]]] = [
        (0, 0, []),
        (100000, 100000, []),
        (3, 4, [(0, 1)]),
        (
            3,
            2,
            [(0, 0), (0, 0), (0, 1), (1, 0), (1, 1), (2, 1)],
        ),
    ]
    rng = random.Random(2026071811)
    for left_size, right_size in [(1, 6), (6, 1), (5, 7), (9, 8)]:
        edges = [
            (rng.randrange(left_size), rng.randrange(right_size))
            for _ in range(rng.randrange(1, 40))
        ]
        cases.append((left_size, right_size, edges))

    edge_count = 100000
    cases.append(
        (
            edge_count,
            edge_count,
            [(vertex, vertex) for vertex in range(edge_count)],
        )
    )
    cases.append((100000, 100000, [(0, 0)] * edge_count))

    regular_size = 33333
    regular = [
        edge
        for left in range(regular_size)
        for edge in (
            (left, left),
            (left, (left + 1) % regular_size),
            (left, (left + 2) % regular_size),
        )
    ]
    cases.append((regular_size, regular_size, regular))

    random_edges = [
        (rng.randrange(100000), rng.randrange(100000))
        for _ in range(edge_count)
    ]
    cases.append((100000, 100000, random_edges))

    for index, case in enumerate(cases):
        write_case(out_dir, index, *case)


if __name__ == "__main__":
    main()
