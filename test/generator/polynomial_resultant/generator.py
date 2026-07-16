#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path

MOD = 998244353


def determinant(matrix: list[list[int]]) -> int:
    size = len(matrix)
    result = 1
    for column in range(size):
        pivot = next(
            (row for row in range(column, size) if matrix[row][column] % MOD),
            None,
        )
        if pivot is None:
            return 0
        if pivot != column:
            matrix[pivot], matrix[column] = matrix[column], matrix[pivot]
            result = -result
        pivot_value = matrix[column][column] % MOD
        result = result * pivot_value % MOD
        inverse = pow(pivot_value, MOD - 2, MOD)
        for row in range(column + 1, size):
            factor = matrix[row][column] * inverse % MOD
            for entry in range(column, size):
                matrix[row][entry] = (
                    matrix[row][entry] - factor * matrix[column][entry]
                ) % MOD
    return result % MOD


def trim(coefficients: list[int]) -> list[int]:
    while coefficients and coefficients[-1] % MOD == 0:
        coefficients.pop()
    return coefficients


def resultant(left: list[int], right: list[int]) -> int:
    left = trim([value % MOD for value in left])
    right = trim([value % MOD for value in right])
    if not left or not right:
        return 0
    left_degree = len(left) - 1
    right_degree = len(right) - 1
    size = left_degree + right_degree
    if size == 0:
        return 1
    matrix = [[0] * size for _ in range(size)]
    for row in range(right_degree):
        for degree, value in enumerate(left):
            matrix[row][row + left_degree - degree] = value
    for offset in range(left_degree):
        row = right_degree + offset
        for degree, value in enumerate(right):
            matrix[row][offset + right_degree - degree] = value
    return determinant(matrix)


def write_case(
    out_dir: Path,
    index: int,
    left: list[int],
    right: list[int],
    answer: int | None = None,
) -> None:
    lines = [f"{len(left)} {len(right)}", " ".join(map(str, left)), " ".join(map(str, right))]
    (out_dir / f"case_{index:02d}.in").write_text(
        "\n".join(lines) + "\n", encoding="utf-8"
    )
    if answer is None:
        answer = resultant(left, right)
    (out_dir / f"case_{index:02d}.out").write_text(
        f"{answer % MOD}\n", encoding="utf-8"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)

    write_case(args.out_dir, 0, [-1, 0, 1], [-2, 1])
    write_case(args.out_dir, 1, [0, 1], [-5, 1])
    write_case(args.out_dir, 2, [3], [7])
    write_case(args.out_dir, 3, [0, 0], [1, 2])
    write_case(args.out_dir, 4, [-2, 1, 1], [-1, 1, -1, 1], 0)

    rng = random.Random(2026071321)
    for index in range(5, 35):
        left_count = rng.randint(1, 8)
        right_count = rng.randint(1, 8)
        left = [rng.randrange(MOD) for _ in range(left_count)]
        right = [rng.randrange(MOD) for _ in range(right_count)]
        if rng.randrange(5) == 0:
            left[-1] = 0
        if rng.randrange(5) == 0:
            right[-1] = 0
        write_case(args.out_dir, index, left, right)

    left = [0] * 20001
    right = [0] * 15001
    left[0] = 3
    left[-1] = 7
    right[-1] = 1
    write_case(args.out_dir, 35, left, right, pow(3, 15000, MOD))


if __name__ == "__main__":
    main()
