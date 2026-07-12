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

    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for matrix, exponent, row, col in cases:
        n = len(matrix)
        input_lines.append(f"{n} {exponent} {row} {col}")
        input_lines.extend(" ".join(map(str, values)) for values in matrix)
        output_lines.append(str(power(matrix, exponent)[row][col]))

    (out_dir / "case_00.in").write_text("\n".join(input_lines) + "\n", encoding="utf-8")
    (out_dir / "case_00.out").write_text("\n".join(output_lines) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
