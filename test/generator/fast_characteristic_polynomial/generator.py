#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path

MOD = 998244353


def multiply_polynomials(left: list[int], right: list[int]) -> list[int]:
    product = [0] * (len(left) + len(right) - 1)
    for i, left_value in enumerate(left):
        for j, right_value in enumerate(right):
            product[i + j] = (product[i + j] + left_value * right_value) % MOD
    return product


def polynomial_from_roots(roots: list[int]) -> list[int]:
    result = [1]
    for root in roots:
        result = multiply_polynomials(result, [(-root) % MOD, 1])
    return result


def characteristic_faddeev(matrix: list[list[int]]) -> list[int]:
    size = len(matrix)
    if size == 0:
        return [1]
    auxiliary = [
        [int(row == column) for column in range(size)] for row in range(size)
    ]
    descending = [1]
    for order in range(1, size + 1):
        product = [[0] * size for _ in range(size)]
        for row in range(size):
            for middle in range(size):
                value = matrix[row][middle]
                for column in range(size):
                    product[row][column] = (
                        product[row][column]
                        + value * auxiliary[middle][column]
                    ) % MOD
        coefficient = -sum(product[index][index] for index in range(size))
        coefficient = coefficient * pow(order, MOD - 2, MOD) % MOD
        descending.append(coefficient)
        for index in range(size):
            product[index][index] = (product[index][index] + coefficient) % MOD
        auxiliary = product
    return list(reversed(descending))


def jordan_blocks(blocks: list[tuple[int, int]]) -> tuple[list[list[int]], list[int]]:
    roots: list[int] = []
    for size, value in blocks:
        roots.extend([value] * size)
    matrix = [[0] * len(roots) for _ in roots]
    offset = 0
    for size, value in blocks:
        for index in range(size):
            matrix[offset + index][offset + index] = value
            if index + 1 < size:
                matrix[offset + index][offset + index + 1] = 1
        offset += size
    return matrix, polynomial_from_roots(roots)


def dense_similar_to_diagonal(
    source: random.Random, size: int
) -> tuple[list[list[int]], list[int]]:
    diagonal = [source.randrange(MOD) for _ in range(size)]
    matrix = [[0] * size for _ in range(size)]
    for index, value in enumerate(diagonal):
        matrix[index][index] = value
    for _ in range(10 * size):
        first = source.randrange(size)
        second = source.randrange(size - 1)
        if second >= first:
            second += 1
        factor = source.randrange(1, MOD)
        for column in range(size):
            matrix[first][column] = (
                matrix[first][column] + factor * matrix[second][column]
            ) % MOD
        for row in range(size):
            matrix[row][second] = (
                matrix[row][second] - factor * matrix[row][first]
            ) % MOD
    return matrix, polynomial_from_roots(diagonal)


def write_case(
    out_dir: Path,
    index: int,
    matrix: list[list[int]],
    expected: list[int] | None = None,
) -> None:
    if expected is None:
        expected = characteristic_faddeev(matrix)
    input_lines = [str(len(matrix))]
    input_lines.extend(" ".join(map(str, row)) for row in matrix)
    (out_dir / f"case_{index:02d}.in").write_text(
        "\n".join(input_lines) + "\n", encoding="utf-8"
    )
    (out_dir / f"case_{index:02d}.out").write_text(
        " ".join(map(str, expected)) + "\n", encoding="utf-8"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[tuple[list[list[int]], list[int] | None]] = [
        ([], [1]),
        ([[0]], [0, 1]),
        ([[7]], [(-7) % MOD, 1]),
        ([[0] * 7 for _ in range(7)], [0] * 7 + [1]),
    ]
    for blocks in [
        [(8, 0)],
        [(9, 13)],
        [(3, 5), (4, 5), (2, 0)],
        [(1, 2), (1, 2), (1, 2), (5, 11)],
    ]:
        cases.append(jordan_blocks(blocks))

    # Large non-generic matrices exercise the recursive rank profile and
    # inversion paths, not only the generic cyclic case.
    for blocks in [
        [(64, 0)],
        [(37, 17), (27, 17)],
        [(19, 0), (23, 5), (22, 5)],
        [(1, 29)] * 64,
    ]:
        matrix, expected = jordan_blocks(blocks)
        lower_jordan = [list(row) for row in zip(*matrix)]
        cases.append((lower_jordan, expected))

    source = random.Random(2026072217)
    for size in range(2, 13):
        for _ in range(5):
            matrix = [
                [source.randrange(MOD) for _ in range(size)]
                for _ in range(size)
            ]
            cases.append((matrix, None))
    for size in [16, 24, 32, 40]:
        for _ in range(2):
            matrix = [
                [source.randrange(MOD) for _ in range(size)]
                for _ in range(size)
            ]
            cases.append((matrix, None))

    for size in range(3, 13):
        matrix = [
            [source.randrange(MOD) for _ in range(size)]
            for _ in range(size)
        ]
        matrix[-1] = matrix[0].copy()
        cases.append((matrix, None))

    cases.append(dense_similar_to_diagonal(source, 128))
    for index, (matrix, expected) in enumerate(cases):
        write_case(args.out_dir, index, matrix, expected)


if __name__ == "__main__":
    main()
