#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def brute(
    capacity: list[int], edges: list[tuple[int, int]]
) -> int:
    answer = 0
    for mask in range(1 << len(edges)):
        degree = [0] * len(capacity)
        size = 0
        for edge_id, (first, second) in enumerate(edges):
            if not (mask >> edge_id & 1):
                continue
            degree[first] += 1
            degree[second] += 1
            size += 1
        if all(left <= right for left, right in zip(degree, capacity)):
            answer = max(answer, size)
    return answer


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    parser.add_argument("--seed", type=int, default=20260728)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(args.seed)
    cases = [
        ([1, 1, 1], [(0, 1), (1, 2), (2, 0)]),
        ([2], [(0, 0), (0, 0)]),
        ([], []),
        ([0, 2], [(0, 1), (1, 1)]),
    ]
    for _ in range(320):
        vertex_count = rng.randint(0, 8)
        capacity = [rng.randint(0, 3) for _ in range(vertex_count)]
        edge_count = rng.randint(0, 12) if vertex_count else 0
        edges = [
            (rng.randrange(vertex_count), rng.randrange(vertex_count))
            for _ in range(edge_count)
        ]
        cases.append((capacity, edges))
    input_lines = [str(len(cases))]
    output_lines = []
    for capacity, edges in cases:
        input_lines.append(f"{len(capacity)} {len(edges)}")
        input_lines.append(" ".join(map(str, capacity)))
        input_lines.extend(f"{first} {second}" for first, second in edges)
        output_lines.append(str(brute(capacity, edges)))
    (args.out_dir / "cases.in").write_text(
        "\n".join(input_lines) + "\n", encoding="utf-8"
    )
    (args.out_dir / "cases.out").write_text(
        "\n".join(output_lines) + "\n", encoding="utf-8"
    )


if __name__ == "__main__":
    main()
