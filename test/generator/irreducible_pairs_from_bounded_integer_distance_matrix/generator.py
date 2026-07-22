#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path

Matrix = list[list[int]]
Case = tuple[int, int, Matrix]


def random_bounded_metric(
    source: random.Random,
    vertex_count: int,
    bound: int,
) -> Matrix:
    if vertex_count == 0:
        return []
    distances = [
        [0 if left == right else source.randint(0, bound)
         for right in range(vertex_count)]
        for left in range(vertex_count)
    ]
    for left in range(vertex_count):
        for right in range(left + 1, vertex_count):
            distances[right][left] = distances[left][right]
    for middle in range(vertex_count):
        for left in range(vertex_count):
            for right in range(vertex_count):
                distances[left][right] = min(
                    distances[left][right],
                    distances[left][middle] + distances[middle][right],
                )
    return distances


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[Case] = [
        (0, 0, []),
        (0, 0, [[0]]),
        (0, 4, [[0, 4], [4, 0]]),
        (0, 0, [[0, 0], [0, 0]]),
        (0, 5, [[0, 2, 5], [2, 0, 3], [5, 3, 0]]),
        (0, 0, [[0, 0, 0], [0, 0, 0], [0, 0, 0]]),
        (0, 10, [[0, 1, 10], [1, 0, 1], [10, 1, 0]]),
        (0, 8, [[0, 3, 4], [3, 0, 8], [4, 8, 0]]),
        (0, 2, [[0, 1], [2, 0]]),
        (0, 2, [[1]]),
        (0, 2, [[0, -1], [-1, 0]]),
        (1, 3, [[0, 4], [4, 0]]),
        (1, -1, []),
        (1, 9_223_372_036_854_775_806,
         [[0, 9_223_372_036_854_775_807],
          [9_223_372_036_854_775_807, 0]]),
        (2, 1, [[0]]),
        (0, 9_223_372_036_854_775_806,
         [[0, 9_223_372_036_854_775_806,
           9_223_372_036_854_775_806],
          [9_223_372_036_854_775_806, 0,
           9_223_372_036_854_775_806],
          [9_223_372_036_854_775_806,
           9_223_372_036_854_775_806, 0]]),
        (1, 9_223_372_036_854_775_807, []),
    ]

    source = random.Random(2026072201)
    for _ in range(420):
        bound = source.randint(0, 9)
        vertex_count = source.randint(0, 24)
        cases.append((
            0,
            bound,
            random_bounded_metric(source, vertex_count, bound),
        ))

    # 128 vertices force the Strassen branch of the exact matrix product.
    cases.append((0, 2, random_bounded_metric(source, 128, 2)))

    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for mode, bound, distances in cases:
        input_lines.append(f"{mode} {bound} {len(distances)}")
        input_lines.extend(" ".join(map(str, row)) for row in distances)
        output_lines.append("OK")

    (out_dir / "case_00.in").write_text(
        "\n".join(input_lines) + "\n",
        encoding="ascii",
    )
    (out_dir / "case_00.out").write_text(
        "\n".join(output_lines) + "\n",
        encoding="ascii",
    )


if __name__ == "__main__":
    main()
