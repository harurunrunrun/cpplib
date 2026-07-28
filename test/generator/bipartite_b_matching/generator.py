#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import itertools
import random
from pathlib import Path


def brute(
    left_capacity: list[int],
    right_capacity: list[int],
    edges: list[tuple[int, int, int]],
) -> int:
    answer = 0
    ranges = [range(capacity + 1) for _, _, capacity in edges]
    for values in itertools.product(*ranges):
        left_degree = [0] * len(left_capacity)
        right_degree = [0] * len(right_capacity)
        for (left, right, _), value in zip(edges, values):
            left_degree[left] += value
            right_degree[right] += value
        if all(a <= b for a, b in zip(left_degree, left_capacity)) and all(
            a <= b for a, b in zip(right_degree, right_capacity)
        ):
            answer = max(answer, sum(values))
    return answer


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    parser.add_argument("--seed", type=int, default=20260728)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(args.seed)
    cases = [
        ([1, 1], [1, 1], [(0, 0, 1), (0, 1, 1), (1, 0, 1)]),
        ([3], [2, 2], [(0, 0, 2), (0, 1, 2)]),
        ([], [], []),
    ]
    for _ in range(220):
        left_size = rng.randint(0, 4)
        right_size = rng.randint(0, 4)
        left_capacity = [rng.randint(0, 4) for _ in range(left_size)]
        right_capacity = [rng.randint(0, 4) for _ in range(right_size)]
        pairs = [
            (left, right)
            for left in range(left_size)
            for right in range(right_size)
        ]
        rng.shuffle(pairs)
        edges = [
            (left, right, rng.randint(0, 3))
            for left, right in pairs[:rng.randint(0, min(7, len(pairs)))]
        ]
        cases.append((left_capacity, right_capacity, edges))
    input_lines = [str(len(cases))]
    output_lines = []
    for left_capacity, right_capacity, edges in cases:
        input_lines.append(
            f"{len(left_capacity)} {len(right_capacity)} {len(edges)}"
        )
        input_lines.append(" ".join(map(str, left_capacity)))
        input_lines.append(" ".join(map(str, right_capacity)))
        input_lines.extend(
            f"{left} {right} {capacity}"
            for left, right, capacity in edges
        )
        output_lines.append(str(brute(left_capacity, right_capacity, edges)))
    (args.out_dir / "cases.in").write_text(
        "\n".join(input_lines) + "\n", encoding="utf-8"
    )
    (args.out_dir / "cases.out").write_text(
        "\n".join(output_lines) + "\n", encoding="utf-8"
    )


if __name__ == "__main__":
    main()
