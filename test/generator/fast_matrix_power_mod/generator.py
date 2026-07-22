#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


Matrix = list[list[int]]
Query = tuple[Matrix, int, int]


def multiply(left: Matrix, right: Matrix, modulus: int) -> Matrix:
    size = len(left)
    result = [[0] * size for _ in range(size)]
    for row in range(size):
        for middle in range(size):
            value = left[row][middle]
            if value == 0:
                continue
            for column in range(size):
                result[row][column] = (
                    result[row][column]
                    + value * right[middle][column]
                ) % modulus
    return result


def matrix_power(matrix: Matrix, exponent: int, modulus: int) -> Matrix:
    size = len(matrix)
    base = [
        [value % modulus for value in row]
        for row in matrix
    ]
    result = [
        [int(row == column) % modulus for column in range(size)]
        for row in range(size)
    ]
    while exponent:
        if exponent & 1:
            result = multiply(result, base, modulus)
        exponent >>= 1
        if exponent:
            base = multiply(base, base, modulus)
    return result


def serialize_query(query: Query) -> str:
    matrix, exponent, modulus = query
    values = [str(value) for row in matrix for value in row]
    return " ".join([
        str(len(matrix)),
        str(exponent),
        str(modulus),
        *values,
    ])


def serialize_answer(query: Query) -> str:
    matrix, exponent, modulus = query
    result = matrix_power(matrix, exponent, modulus)
    return " ".join(str(value) for row in result for value in row)


def write_case(out_dir: Path, name: str, queries: list[Query]) -> None:
    (out_dir / f"{name}.in").write_text(
        "\n".join([str(len(queries)), *(serialize_query(q) for q in queries)])
        + "\n",
        encoding="utf-8",
    )
    (out_dir / f"{name}.out").write_text(
        "\n".join(serialize_answer(query) for query in queries) + "\n",
        encoding="utf-8",
    )


def fixed_queries() -> list[Query]:
    return [
        ([[5]], 0, 1),
        ([[-2]], 13, 97),
        ([[1, 1], [1, 0]], 50, 1_000_000_007),
        (
            [[1, -2, 3], [4, 5, -6], [7, 8, 9]],
            17,
            998_244_353,
        ),
    ]


def random_queries(seed: int, count: int, maximum_size: int) -> list[Query]:
    random_engine = random.Random(seed)
    moduli = [
        2,
        3,
        1_000_000_007,
        4_294_967_291,
        9_223_372_036_854_775_783,
    ]
    queries: list[Query] = []
    for _ in range(count):
        size = random_engine.randint(1, maximum_size)
        modulus = random_engine.choice(moduli)
        exponent = random_engine.randrange(0, 80)
        matrix = [
            [
                random_engine.randrange(
                    -9_223_372_036_854_775_808,
                    9_223_372_036_854_775_808,
                )
                for _ in range(size)
            ]
            for _ in range(size)
        ]
        queries.append((matrix, exponent, modulus))
    return queries


def strassen_query() -> Query:
    size = 96
    modulus = 1_000_000_007
    matrix = [
        [
            (
                row * 1_000_003
                + column * 97
                + (row ^ column) * 65_537
                + 11
            ) % modulus
            for column in range(size)
        ]
        for row in range(size)
    ]
    return matrix, 3, modulus


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)
    write_case(args.out_dir, "fixed", fixed_queries())
    write_case(
        args.out_dir,
        "random_small",
        random_queries(20260722, 24, 8),
    )
    write_case(
        args.out_dir,
        "large_modulus",
        random_queries(0x9E3779B97F4A7C15, 8, 12),
    )
    write_case(args.out_dir, "strassen", [strassen_query()])


if __name__ == "__main__":
    main()
