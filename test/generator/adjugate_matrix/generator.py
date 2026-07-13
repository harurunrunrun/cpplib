#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path

MOD = 998244353


def determinant(matrix: list[list[int]]) -> int:
    size = len(matrix)
    values = [row[:] for row in matrix]
    result = 1
    for column in range(size):
        pivot = next((row for row in range(column, size) if values[row][column]), None)
        if pivot is None:
            return 0
        if pivot != column:
            values[pivot], values[column] = values[column], values[pivot]
            result = -result
        pivot_value = values[column][column]
        result = result * pivot_value % MOD
        inverse = pow(pivot_value, MOD - 2, MOD)
        for row in range(column + 1, size):
            factor = values[row][column] * inverse % MOD
            for entry in range(column, size):
                values[row][entry] = (
                    values[row][entry] - factor * values[column][entry]
                ) % MOD
    return result % MOD


def naive_adjugate(matrix: list[list[int]]) -> list[list[int]]:
    size = len(matrix)
    result = [[0] * size for _ in range(size)]
    for row in range(size):
        for column in range(size):
            minor = [
                [matrix[source_row][source_column]
                 for source_column in range(size) if source_column != row]
                for source_row in range(size) if source_row != column
            ]
            value = determinant(minor)
            if (row + column) & 1:
                value = -value
            result[row][column] = value % MOD
    return result


def write_case(
    out_dir: Path,
    index: int,
    matrix: list[list[int]],
    answer: list[list[int]],
) -> None:
    with (out_dir / f"case_{index:02d}.in").open(
        "w", encoding="utf-8", newline="\n"
    ) as output:
        output.write(f"{len(matrix)}\n")
        output.writelines(" ".join(map(str, row)) + "\n" for row in matrix)
    (out_dir / f"case_{index:02d}.out").write_text(
        "".join(" ".join(map(str, row)) + "\n" for row in answer),
        encoding="utf-8",
    )


def rank_one_update_case(
    source: random.Random,
    size: int,
    singular: bool,
) -> tuple[list[list[int]], list[list[int]]]:
    left = [source.randrange(1, MOD) for _ in range(size)]
    right = [source.randrange(1, MOD) for _ in range(size)]
    dot = sum(a * b for a, b in zip(left, right)) % MOD
    if singular:
        right[0] = (right[0] + (1 - dot) * pow(left[0], MOD - 2, MOD)) % MOD
        matrix = [
            [(int(row == column) - left[row] * right[column]) % MOD
             for column in range(size)]
            for row in range(size)
        ]
        answer = [
            [left[row] * right[column] % MOD for column in range(size)]
            for row in range(size)
        ]
        return matrix, answer

    determinant_value = (1 + dot) % MOD
    if determinant_value == 0:
        right[0] = (right[0] + pow(left[0], MOD - 2, MOD)) % MOD
        dot = sum(a * b for a, b in zip(left, right)) % MOD
        determinant_value = (1 + dot) % MOD
    matrix = [
        [(int(row == column) + left[row] * right[column]) % MOD
         for column in range(size)]
        for row in range(size)
    ]
    answer = [
        [
            (determinant_value * int(row == column) - left[row] * right[column])
            % MOD
            for column in range(size)
        ]
        for row in range(size)
    ]
    return matrix, answer


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)

    cases = [[[0]], [[7]], [[1, 2], [3, 4]], [[1, 2], [2, 4]]]
    for index, matrix in enumerate(cases):
        write_case(args.out_dir, index, matrix, naive_adjugate(matrix))

    source = random.Random(2026071319)
    index = len(cases)
    for size in range(2, 9):
        for _ in range(5):
            matrix = [[source.randrange(MOD) for _ in range(size)] for _ in range(size)]
            write_case(args.out_dir, index, matrix, naive_adjugate(matrix))
            index += 1


        rank_one_deficient = [
            [source.randrange(MOD) for _ in range(size)] for _ in range(size - 1)
        ]
        coefficients = [source.randrange(MOD) for _ in range(size - 1)]
        rank_one_deficient.append([
            sum(coefficients[row] * rank_one_deficient[row][column]
                for row in range(size - 1)) % MOD
            for column in range(size)
        ])
        source.shuffle(rank_one_deficient)
        column_order = list(range(size))
        source.shuffle(column_order)
        rank_one_deficient = [
            [row[column] for column in column_order] for row in rank_one_deficient
        ]
        write_case(
            args.out_dir,
            index,
            rank_one_deficient,
            naive_adjugate(rank_one_deficient),
        )
        index += 1

        left_factor = [
            [source.randrange(MOD) for _ in range(size - 2)] for _ in range(size)
        ]
        right_factor = [
            [source.randrange(MOD) for _ in range(size)] for _ in range(size - 2)
        ]
        rank_two_deficient = [
            [
                sum(left_factor[row][middle] * right_factor[middle][column]
                    for middle in range(size - 2)) % MOD
                for column in range(size)
            ]
            for row in range(size)
        ]
        write_case(
            args.out_dir,
            index,
            rank_two_deficient,
            naive_adjugate(rank_two_deficient),
        )
        index += 1
    matrix, answer = rank_one_update_case(source, 500, False)
    write_case(args.out_dir, index, matrix, answer)
    index += 1
    matrix, answer = rank_one_update_case(source, 500, True)
    write_case(args.out_dir, index, matrix, answer)
    index += 1
    matrix = [[0] * 500 for _ in range(500)]
    for diagonal in range(498):
        matrix[diagonal][diagonal] = diagonal + 1
    write_case(args.out_dir, index, matrix, [[0] * 500 for _ in range(500)])


if __name__ == "__main__":
    main()
