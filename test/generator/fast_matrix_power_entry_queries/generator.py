#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


Matrix = list[list[int]]
Query = tuple[int, int, int]
Case = tuple[Matrix, int, list[Query], bool]


def multiply(left: Matrix, right: Matrix, modulus: int) -> Matrix:
    size = len(left)
    result = [[0] * size for _ in range(size)]
    for row in range(size):
        for middle in range(size):
            coefficient = left[row][middle]
            if coefficient == 0:
                continue
            for column in range(size):
                result[row][column] = (
                    result[row][column]
                    + coefficient * right[middle][column]
                ) % modulus
    return result


def matrix_power(matrix: Matrix, exponent: int, modulus: int) -> Matrix:
    size = len(matrix)
    base = [[value % modulus for value in row] for row in matrix]
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


def fixed_cases() -> list[Case]:
    return [
        ([], 7, [], False),
        ([[5]], 1, [(0, 0, 0), (0, 0, 2**64 - 1)], False),
        (
            [[1, 1], [1, 0]],
            1_000_000_007,
            [(0, 0, 0), (0, 1, 0), (0, 0, 1), (0, 1, 50)],
            False,
        ),
        (
            [
                [-(2**63), 2**63 - 1, -7],
                [13, -17, 19],
                [23, 29, -31],
            ],
            2**63 - 25,
            [
                (0, 0, 0),
                (2, 1, 1),
                (1, 2, 17),
                (0, 2, 2**63),
                (2, 0, 2**64 - 1),
            ],
            False,
        ),
    ]


def random_cases(seed: int, count: int) -> list[Case]:
    rng = random.Random(seed)
    moduli = [2, 3, 97, 998_244_353, 4_294_967_291, 2**63 - 25, 2**63 - 1]
    exponent_pool = [0, 1, 2, 3, 7, 63, 2**32 + 15, 2**63, 2**64 - 1]
    cases: list[Case] = []
    for _ in range(count):
        size = rng.randrange(1, 8)
        modulus = rng.choice(moduli)
        matrix = [
            [rng.randrange(-(2**63), 2**63) for _ in range(size)]
            for _ in range(size)
        ]
        queries = [
            (
                rng.randrange(size),
                rng.randrange(size),
                rng.choice(exponent_pool + [rng.randrange(2**64)]),
            )
            for _ in range(10)
        ]
        cases.append((matrix, modulus, queries, False))
    return cases


def strassen_case() -> Case:
    # 128 is the first square size for which the shared implementation enters
    # its Strassen recursion. Selected entries of A^2 need only a linear dot
    # product in the oracle; randomized small cases above use the full cubic
    # oracle for arbitrary 64-bit exponents.
    size = 128
    modulus = 2**63 - 25
    matrix = [
        [
            (
                row * 1_000_003
                + column * 65_537
                + (row ^ column) * 97
                + 11
            )
            % (2**63)
            - 2**62
            for column in range(size)
        ]
        for row in range(size)
    ]
    queries = [
        (0, 0, 0),
        (0, 1, 0),
        (1, 7, 1),
        (0, 0, 2),
        (37, 91, 2),
        (127, 126, 2),
    ]
    return matrix, modulus, queries, True


def serialize_case(case: Case) -> list[str]:
    matrix, modulus, queries, _ = case
    lines = [f"{len(matrix)} {modulus} {len(queries)}"]
    if matrix:
        lines.append(" ".join(str(value) for row in matrix for value in row))
    lines.extend(f"{row} {column} {exponent}" for row, column, exponent in queries)
    return lines


def answers(case: Case) -> list[int]:
    matrix, modulus, queries, strassen = case
    if strassen:
        result: list[int] = []
        for row, column, exponent in queries:
            if exponent == 0:
                result.append(int(row == column) % modulus)
            elif exponent == 1:
                result.append(matrix[row][column] % modulus)
            else:
                assert exponent == 2
                result.append(sum(
                    matrix[row][middle] * matrix[middle][column]
                    for middle in range(len(matrix))
                ) % modulus)
        return result

    powers: dict[int, Matrix] = {}
    result: list[int] = []
    for row, column, exponent in queries:
        if exponent not in powers:
            powers[exponent] = matrix_power(matrix, exponent, modulus)
        result.append(powers[exponent][row][column])
    return result


def write_case(out_dir: Path, name: str, cases: list[Case]) -> None:
    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for case in cases:
        input_lines.extend(serialize_case(case))
        output_lines.extend(str(value) for value in answers(case))
    (out_dir / f"{name}.in").write_text(
        "\n".join(input_lines) + "\n", encoding="utf-8"
    )
    (out_dir / f"{name}.out").write_text(
        "\n".join(output_lines) + ("\n" if output_lines else ""),
        encoding="utf-8",
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    arguments = parser.parse_args()
    arguments.out_dir.mkdir(parents=True, exist_ok=True)
    write_case(arguments.out_dir, "fixed", fixed_cases())
    write_case(
        arguments.out_dir,
        "random_small",
        random_cases(0x9E3779B97F4A7C15, 24),
    )
    write_case(arguments.out_dir, "strassen", [strassen_case()])


if __name__ == "__main__":
    main()
