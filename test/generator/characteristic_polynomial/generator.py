#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path

MOD = 998244353


def characteristic_faddeev(matrix: list[list[int]]) -> list[int]:
    size = len(matrix)
    if size == 0:
        return [1]
    auxiliary = [[int(row == column) for column in range(size)] for row in range(size)]
    descending = [1]
    for order in range(1, size + 1):
        product = [[0] * size for _ in range(size)]
        for row in range(size):
            for middle in range(size):
                left = matrix[row][middle]
                for column in range(size):
                    product[row][column] = (
                        product[row][column] + left * auxiliary[middle][column]
                    ) % MOD
        coefficient = -sum(product[index][index] for index in range(size))
        coefficient = coefficient * pow(order, MOD - 2, MOD) % MOD
        descending.append(coefficient)
        for index in range(size):
            product[index][index] = (product[index][index] + coefficient) % MOD
        auxiliary = product
    return list(reversed(descending))


def polynomial_from_roots(roots: list[int]) -> list[int]:
    result = [1]
    for root in roots:
        next_result = [0] * (len(result) + 1)
        for degree, value in enumerate(result):
            next_result[degree] = (next_result[degree] - root * value) % MOD
            next_result[degree + 1] = (next_result[degree + 1] + value) % MOD
        result = next_result
    return result


def write_case(
    out_dir: Path,
    index: int,
    matrix: list[list[int]],
    answer: list[int] | None = None,
) -> None:
    if answer is None:
        answer = characteristic_faddeev(matrix)
    with (out_dir / f"case_{index:02d}.in").open(
        "w", encoding="utf-8", newline="\n"
    ) as output:
        output.write(f"{len(matrix)}\n")
        output.writelines(" ".join(map(str, row)) + "\n" for row in matrix)
    (out_dir / f"case_{index:02d}.out").write_text(
        " ".join(map(str, answer)) + "\n", encoding="utf-8"
    )


def dense_similar_to_diagonal(
    random_source: random.Random,
    size: int,
) -> tuple[list[list[int]], list[int]]:
    diagonal = [random_source.randrange(MOD) for _ in range(size)]
    matrix = [[0] * size for _ in range(size)]
    for index, value in enumerate(diagonal):
        matrix[index][index] = value
    for _ in range(8 * size):
        first = random_source.randrange(size)
        second = random_source.randrange(size - 1)
        if second >= first:
            second += 1
        factor = random_source.randrange(1, MOD)
        for column in range(size):
            matrix[first][column] = (
                matrix[first][column] + factor * matrix[second][column]
            ) % MOD
        for row in range(size):
            matrix[row][second] = (
                matrix[row][second] - factor * matrix[row][first]
            ) % MOD
    return matrix, polynomial_from_roots(diagonal)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)

    write_case(args.out_dir, 0, [])
    write_case(args.out_dir, 1, [[7]])
    write_case(args.out_dir, 2, [[1, 2], [3, 4]])
    write_case(args.out_dir, 3, [[0, 1, 0], [0, 0, 1], [6, 5, 4]])

    random_source = random.Random(2026071312)
    index = 4
    for size in range(2, 9):
        for _ in range(5):
            matrix = [
                [random_source.randrange(MOD) for _ in range(size)]
                for _ in range(size)
            ]
            write_case(args.out_dir, index, matrix)
            index += 1

    matrix, answer = dense_similar_to_diagonal(random_source, 500)
    write_case(args.out_dir, index, matrix, answer)


if __name__ == "__main__":
    main()
