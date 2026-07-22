#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path

MOD = 998244353


def determinant(matrix: list[list[int]]) -> int:
    size = len(matrix)
    values = [row.copy() for row in matrix]
    answer = 1
    for column in range(size):
        pivot = next(
            (row for row in range(column, size) if values[row][column]),
            None,
        )
        if pivot is None:
            return 0
        if pivot != column:
            values[pivot], values[column] = values[column], values[pivot]
            answer = -answer
        pivot_value = values[column][column]
        answer = answer * pivot_value % MOD
        inverse = pow(pivot_value, MOD - 2, MOD)
        for row in range(column + 1, size):
            factor = values[row][column] * inverse % MOD
            for entry in range(column + 1, size):
                values[row][entry] = (
                    values[row][entry] - factor * values[column][entry]
                ) % MOD
    return answer % MOD


def cauchy(size: int) -> tuple[list[list[int]], int]:
    left = [index + 1 for index in range(size)]
    right = [1000 + index for index in range(size)]
    matrix = [
        [pow(x + y, MOD - 2, MOD) for y in right]
        for x in left
    ]
    numerator = 1
    denominator = 1
    for first in range(size):
        for second in range(first + 1, size):
            numerator = numerator * (left[second] - left[first]) % MOD
            numerator = numerator * (right[second] - right[first]) % MOD
    for x in left:
        for y in right:
            denominator = denominator * (x + y) % MOD
    expected = numerator * pow(denominator, MOD - 2, MOD) % MOD
    return matrix, expected


def write_case(
    out_dir: Path,
    index: int,
    matrix: list[list[int]],
    expected: int | None = None,
) -> None:
    if expected is None:
        expected = determinant(matrix)
    lines = [str(len(matrix))]
    lines.extend(" ".join(map(str, row)) for row in matrix)
    (out_dir / f"case_{index:02d}.in").write_text(
        "\n".join(lines) + "\n", encoding="utf-8"
    )
    (out_dir / f"case_{index:02d}.out").write_text(
        f"{expected}\n", encoding="utf-8"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[tuple[list[list[int]], int | None]] = [
        ([], 1),
        ([[0]], 0),
        ([[7]], 7),
        ([[0, 1], [1, 0]], MOD - 1),
        ([[0, 1, 0], [0, 0, 1], [1, 0, 0]], 1),
        ([[1, 2, 3], [2, 4, 6], [7, 8, 9]], 0),
    ]
    source = random.Random(2026072241)
    for size in range(2, 14):
        for _ in range(5):
            matrix = [
                [source.randrange(MOD) for _ in range(size)]
                for _ in range(size)
            ]
            cases.append((matrix, None))
        singular = [
            [source.randrange(MOD) for _ in range(size)]
            for _ in range(size)
        ]
        singular[-1] = singular[0].copy()
        cases.append((singular, 0))

    # Exercise recursive rank profiles, including both permutation parities.
    for size in [64, 65]:
        permutation = [((index + 1) % size) for index in range(size)]
        matrix = [[0] * size for _ in range(size)]
        for row, column in enumerate(permutation):
            matrix[row][column] = 1
        cases.append((matrix, MOD - 1 if size % 2 == 0 else 1))
    for size in [33, 48]:
        matrix = [
            [source.randrange(MOD) for _ in range(size)]
            for _ in range(size)
        ]
        cases.append((matrix, None))
    singular = [
        [int(row == column) for column in range(64)] for row in range(64)
    ]
    singular[-1] = singular[0].copy()
    cases.append((singular, 0))

    dense, expected = cauchy(256)
    cases.append((dense, expected))
    for index, (matrix, answer) in enumerate(cases):
        write_case(args.out_dir, index, matrix, answer)


if __name__ == "__main__":
    main()
