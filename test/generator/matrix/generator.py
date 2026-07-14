#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path

MOD = 998244353


def multiply(a: list[list[int]], b: list[list[int]]) -> list[list[int]]:
    rows = len(a)
    inner = len(b)
    cols = len(b[0])
    result = [[0] * cols for _ in range(rows)]
    for i in range(rows):
        for k in range(inner):
            for j in range(cols):
                result[i][j] = (result[i][j] + a[i][k] * b[k][j]) % MOD
    return result


def power(base: list[list[int]], exponent: int) -> list[list[int]]:
    n = len(base)
    result = [[int(i == j) for j in range(n)] for i in range(n)]
    while exponent:
        if exponent & 1:
            result = multiply(result, base)
        base = multiply(base, base)
        exponent >>= 1
    return result


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()

    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[tuple[list[list[int]], int, int, int]] = [
        ([[0]], 0, 0, 0),
        ([[0]], 10**18, 0, 0),
        ([[1, 1], [1, 0]], 10**18, 0, 1),
        ([[1, -3], [2, -6]], 827141057288478, 0, 0),
    ]
    rng = random.Random(20260713)
    for n in range(2, 7):
        for _ in range(4):
            matrix = [[rng.randrange(-20, 21) for _ in range(n)] for _ in range(n)]
            exponent = rng.choice([rng.randrange(0, 30), rng.randrange(10**12, 10**15)])
            row = rng.randrange(n)
            col = rng.randrange(n)
            cases.append((matrix, exponent, row, col))

    multiply_cases: list[tuple[list[list[int]], list[list[int]]]] = []
    for _ in range(40):
        rows = rng.randrange(1, 14)
        inner = rng.randrange(1, 14)
        cols = rng.randrange(1, 14)
        lhs = [[rng.randrange(-2 * MOD, 2 * MOD + 1) for _ in range(inner)]
               for _ in range(rows)]
        rhs = [[rng.randrange(-2 * MOD, 2 * MOD + 1) for _ in range(cols)]
               for _ in range(inner)]
        multiply_cases.append((lhs, rhs))
    large_n = 128
    large_lhs = [[rng.randrange(-1000, 1001) for _ in range(large_n)]
                 for _ in range(large_n)]
    large_rhs = [[rng.randrange(-1000, 1001) for _ in range(large_n)]
                 for _ in range(large_n)]
    multiply_cases.append((large_lhs, large_rhs))

    # Exercise each independent compile-time dimension at the official limit
    # without making the standalone oracle cubic.
    limit = 1024
    multiply_cases.extend([
        (
            [[i % 17 - 8] for i in range(limit)],
            [[7, -11]],
        ),
        (
            [[i % 23 - 11 for i in range(limit)]],
            [[i % 19 - 9] for i in range(limit)],
        ),
        (
            [[3], [-5]],
            [[j % 29 - 14 for j in range(limit)]],
        ),
    ])

    input_lines = [str(len(cases) + len(multiply_cases))]
    output_lines: list[str] = []
    for matrix, exponent, row, col in cases:
        n = len(matrix)
        input_lines.append(f"BMBM {n} {exponent} {row} {col}")
        input_lines.extend(" ".join(map(str, values)) for values in matrix)
        output_lines.append(str(power(matrix, exponent)[row][col]))

    for lhs, rhs in multiply_cases:
        rows = len(lhs)
        inner = len(rhs)
        cols = len(rhs[0])
        input_lines.append(f"MUL {rows} {inner} {cols}")
        input_lines.extend(" ".join(map(str, values)) for values in lhs)
        input_lines.extend(" ".join(map(str, values)) for values in rhs)
        product = multiply(lhs, rhs)
        output_lines.append(" ".join(map(str, [
            rows, cols, *(value for values in product for value in values)
        ])))

    (out_dir / "case_00.in").write_text("\n".join(input_lines) + "\n", encoding="utf-8")
    (out_dir / "case_00.out").write_text("\n".join(output_lines) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
