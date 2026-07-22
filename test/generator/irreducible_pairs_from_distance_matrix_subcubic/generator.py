#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path

Matrix = list[list[int]]
Case = tuple[int, Matrix]
MAXIMUM = 9_223_372_036_854_775_807


def metric_closure(matrix: Matrix) -> Matrix:
    size = len(matrix)
    for middle in range(size):
        for left in range(size):
            for right in range(size):
                matrix[left][right] = min(
                    matrix[left][right],
                    matrix[left][middle] + matrix[middle][right],
                )
    return matrix


def random_metric(
    source: random.Random,
    vertex_count: int,
    maximum_edge: int,
) -> Matrix:
    matrix = [[0] * vertex_count for _ in range(vertex_count)]
    for left in range(vertex_count):
        for right in range(left + 1, vertex_count):
            distance = source.randint(0, maximum_edge)
            matrix[left][right] = matrix[right][left] = distance
    return metric_closure(matrix)


def line_metric(coordinates: list[int]) -> Matrix:
    return [
        [abs(left - right) for right in coordinates]
        for left in coordinates
    ]


def discrete_metric(vertex_count: int, distance: int) -> Matrix:
    return [
        [0 if left == right else distance for right in range(vertex_count)]
        for left in range(vertex_count)
    ]


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[Case] = [
        (0, []),
        (0, [[0]]),
        (0, [[0, MAXIMUM], [MAXIMUM, 0]]),
        (0, discrete_metric(3, MAXIMUM)),
        (0, line_metric([0, 1, 1 << 62, MAXIMUM])),
        (0, [[0, 0, 0], [0, 0, 0], [0, 0, 0]]),
        (0, line_metric([0, 1, 4, 9, 15, 22])),
        (0, [[0, 2, 5], [2, 0, 3], [5, 3, 0]]),
        (1, [[1]]),
        (1, [[0, -1], [-1, 0]]),
        (1, [[0, 1], [2, 0]]),
        (1, [[0, 1, 10], [1, 0, 1], [10, 1, 0]]),
        (2, [[0]]),
    ]

    source = random.Random(2026072202)
    bounds = [0, 17, 10**9, 10**18, MAXIMUM]
    for _ in range(320):
        vertex_count = source.randint(0, 30)
        cases.append((
            0,
            random_metric(
                source,
                vertex_count,
                source.choice(bounds),
            ),
        ))
    for _ in range(80):
        vertex_count = source.randint(0, 45)
        coordinates = sorted(
            source.randrange(MAXIMUM + 1)
            for _ in range(vertex_count)
        )
        cases.append((0, line_metric(coordinates)))

    # floor(log2(260)) / 4 == 2, so these cases exercise a genuine
    # two-candidate Chan block rather than the one-candidate base case.
    cases.append((0, line_metric(list(range(260)))))
    cases.append((0, discrete_metric(260, MAXIMUM)))

    lines = [str(len(cases))]
    expected: list[str] = []
    for mode, matrix in cases:
        lines.append(f"{mode} {len(matrix)}")
        lines.extend(" ".join(map(str, row)) for row in matrix)
        expected.append("OK")

    (out_dir / "case_00.in").write_text(
        "\n".join(lines) + "\n",
        encoding="ascii",
    )
    (out_dir / "case_00.out").write_text(
        "\n".join(expected) + "\n",
        encoding="ascii",
    )


if __name__ == "__main__":
    main()
