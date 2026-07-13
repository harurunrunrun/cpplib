#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def matching_size(
    left_size: int, right_size: int, edges: list[tuple[int, int]]
) -> int:
    graph: list[list[int]] = [[] for _ in range(left_size)]
    for left, right in edges:
        graph[left].append(right)
    right_match = [-1] * right_size

    def augment(left: int, seen: list[bool]) -> bool:
        for right in graph[left]:
            if seen[right]:
                continue
            seen[right] = True
            if right_match[right] == -1 or augment(right_match[right], seen):
                right_match[right] = left
                return True
        return False

    result = 0
    for left in range(left_size):
        result += augment(left, [False] * right_size)
    return result


def write_case(
    out_dir: Path,
    index: int,
    left_size: int,
    right_size: int,
    edges: list[tuple[int, int]],
) -> None:
    size = matching_size(left_size, right_size, edges)
    left_degree = [0] * left_size
    right_degree = [0] * right_size
    for left, right in edges:
        left_degree[left] += 1
        right_degree[right] += 1
    edge_cover = (
        left_size + right_size - size
        if all(left_degree) and all(right_degree)
        else -1
    )
    lines = [f"{left_size} {right_size} {len(edges)}"]
    lines.extend(f"{left} {right}" for left, right in edges)
    (out_dir / f"case_{index:02d}.in").write_text(
        "\n".join(lines) + "\n", encoding="utf-8"
    )
    (out_dir / f"case_{index:02d}.out").write_text(
        f"{size}\n{size}\n{left_size + right_size - size}\n{edge_cover}\n",
        encoding="utf-8",
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[tuple[int, int, list[tuple[int, int]]]] = [
        (0, 0, []),
        (1, 1, []),
        (2, 3, [(0, 0), (0, 1), (1, 1), (1, 2)]),
        (4, 4, [(left, right) for left in range(4) for right in range(4)]),
        (3, 5, [(0, 0), (1, 1), (2, 2), (2, 3), (2, 4)]),
    ]
    rng = random.Random(2026071311)
    for _ in range(35):
        left_size = rng.randrange(0, 25)
        right_size = rng.randrange(0, 25)
        edges = [
            (left, right)
            for left in range(left_size)
            for right in range(right_size)
            if rng.randrange(5) == 0
        ]
        cases.append((left_size, right_size, edges))

    size = 5000
    cases.append((size, size, [(vertex, vertex) for vertex in range(size)]))
    for index, (left_size, right_size, edges) in enumerate(cases):
        write_case(args.out_dir, index, left_size, right_size, edges)


if __name__ == "__main__":
    main()
