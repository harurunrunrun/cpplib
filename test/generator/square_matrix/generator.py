#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path

MOD = 998244353


def multiply(a: list[list[int]], b: list[list[int]]) -> list[list[int]]:
    n = len(a)
    result = [[0] * n for _ in range(n)]
    for i in range(n):
        for k in range(n):
            for j in range(n):
                result[i][j] = (result[i][j] + a[i][k] * b[k][j]) % MOD
    return result


def matrix_power(matrix: list[list[int]], exponent: int) -> list[list[int]]:
    n = len(matrix)
    result = [[int(i == j) for j in range(n)] for i in range(n)]
    base = [[value % MOD for value in row] for row in matrix]
    while exponent:
        if exponent & 1:
            result = multiply(result, base)
        exponent >>= 1
        if exponent:
            base = multiply(base, base)
    return result


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(20260713)
    cases: list[tuple[list[list[int]], int]] = [
        ([], 0),
        ([[7]], 0),
        ([[-3]], 9_000_000_000_000_000_000),
        ([[1, 1], [1, 0]], 10),
        ([[0, 1], [0, 0]], 10**18),
        ([[1, -3], [2, -6]], 1_000_000_000_000_000_003),
    ]
    for _ in range(90):
        n = rng.randrange(1, 6)
        exponent = rng.randrange(31) if rng.randrange(3) else rng.randrange(10**18 + 1)
        matrix = [[rng.randrange(-2 * MOD, 2 * MOD + 1) for _ in range(n)] for _ in range(n)]
        cases.append((matrix, exponent))

    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for matrix, exponent in cases:
        n = len(matrix)
        input_lines.append(f"{n} {exponent}")
        input_lines.extend(" ".join(map(str, row)) for row in matrix)
        power = matrix_power(matrix, exponent)
        output_lines.append(" ".join(map(str, [n, *(value for row in power for value in row)])))

    (out_dir / "case_00.in").write_text("\n".join(input_lines) + "\n", encoding="utf-8")
    (out_dir / "case_00.out").write_text("\n".join(output_lines) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
