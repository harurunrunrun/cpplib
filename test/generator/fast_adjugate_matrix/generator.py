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


def adjugate(matrix: list[list[int]]) -> list[list[int]]:
    size = len(matrix)
    if size == 0:
        return []
    result = [[0] * size for _ in range(size)]
    for row in range(size):
        for column in range(size):
            minor = [
                [
                    matrix[source_row][source_column]
                    for source_column in range(size)
                    if source_column != row
                ]
                for source_row in range(size)
                if source_row != column
            ]
            value = determinant(minor)
            if (row + column) & 1:
                value = -value
            result[row][column] = value % MOD
    return result


def matrix_rank(matrix: list[list[int]]) -> int:
    values = [row.copy() for row in matrix]
    rows = len(values)
    columns = len(values[0]) if rows else 0
    rank = 0
    for column in range(columns):
        pivot = next(
            (row for row in range(rank, rows) if values[row][column]),
            None,
        )
        if pivot is None:
            continue
        values[rank], values[pivot] = values[pivot], values[rank]
        inverse = pow(values[rank][column], MOD - 2, MOD)
        for row in range(rank + 1, rows):
            factor = values[row][column] * inverse % MOD
            for entry in range(column, columns):
                values[row][entry] = (
                    values[row][entry] - factor * values[rank][entry]
                ) % MOD
        rank += 1
    return rank


def random_of_rank(
    source: random.Random,
    size: int,
    target_rank: int,
) -> list[list[int]]:
    while True:
        left = [
            [source.randrange(MOD) for _ in range(target_rank)]
            for _ in range(size)
        ]
        right = [
            [source.randrange(MOD) for _ in range(size)]
            for _ in range(target_rank)
        ]
        matrix = [
            [
                sum(left[row][middle] * right[middle][column]
                    for middle in range(target_rank)) % MOD
                for column in range(size)
            ]
            for row in range(size)
        ]
        if matrix_rank(matrix) == target_rank:
            return matrix


def write_case(
    out_dir: Path,
    index: int,
    matrix: list[list[int]],
    expected: list[list[int]] | None = None,
) -> None:
    if expected is None:
        expected = adjugate(matrix)
    lines = [str(len(matrix))]
    lines.extend(" ".join(map(str, row)) for row in matrix)
    output = [" ".join(map(str, row)) for row in expected]
    (out_dir / f"case_{index:02d}.in").write_text(
        "\n".join(lines) + "\n", encoding="utf-8"
    )
    (out_dir / f"case_{index:02d}.out").write_text(
        ("\n".join(output) + "\n") if output else "", encoding="utf-8"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[tuple[list[list[int]], list[list[int]] | None]] = [
        ([], []),
        ([[0]], [[1]]),
        ([[7]], [[1]]),
        ([[1, 2], [3, 4]], None),
        ([[0, 1, 0], [0, 0, 1], [0, 0, 0]], None),
        ([[1, 0, 0], [0, 0, 0], [0, 0, 0]], None),
        ([[0, 0, 1], [1, 0, 0], [0, 1, 0]], None),
    ]
    source = random.Random(2026072273)
    for size in range(2, 9):
        for target_rank in (size, size - 1, max(0, size - 2)):
            for _ in range(3):
                cases.append((
                    random_of_rank(source, size, target_rank),
                    None,
                ))

    # A 256-cycle is invertible and forces 128 x 128 recursive products.
    size = 256
    cyclic = [[0] * size for _ in range(size)]
    inverse = [[0] * size for _ in range(size)]
    for row in range(size):
        cyclic[row][(row + 1) % size] = 1
        inverse[(row + 1) % size][row] = MOD - 1
    cases.append((cyclic, inverse))

    for index, (matrix, expected) in enumerate(cases):
        write_case(args.out_dir, index, matrix, expected)


if __name__ == "__main__":
    main()
