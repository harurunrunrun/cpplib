#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path

Matrix = list[list[int]]
Case = tuple[int, int, Matrix]


def random_metric(
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
        (0, 7, [[0, 7], [7, 0]]),
        (0, 0, [[0, 0], [0, 0]]),
        (0, 6, [[0, 2, 6], [2, 0, 4], [6, 4, 0]]),
        (0, 8, [[0, 3, 5], [3, 0, 8], [5, 8, 0]]),
        (0, 0, [[0, 0, 0], [0, 0, 0], [0, 0, 0]]),
        (1, -1, []),
        (1, 4_194_304, []),
        (1, 2, [[1]]),
        (1, 2, [[0, -1], [-1, 0]]),
        (1, 2, [[0, 3], [3, 0]]),
        (1, 2, [[0, 1], [2, 0]]),
        (2, 1, [[0]]),
    ]

    source = random.Random(2026072202)
    for _ in range(240):
        bound = source.randint(0, 9)
        vertex_count = source.randint(0, 22)
        cases.append((
            0,
            bound,
            random_metric(source, vertex_count, bound),
        ))

    cases.append((0, 1, random_metric(source, 96, 1)))

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
