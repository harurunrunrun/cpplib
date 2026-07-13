#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import functools
import random
from pathlib import Path

MOD = 998244353


def pfaffian_brute(matrix: list[list[int]]) -> int:
    size = len(matrix)

    @functools.cache
    def solve(mask: int) -> int:
        if mask == 0:
            return 1
        first = (mask & -mask).bit_length() - 1
        rest = mask ^ (1 << first)
        answer = 0
        position = 0
        bits = rest
        while bits:
            second = (bits & -bits).bit_length() - 1
            position += 1
            term = matrix[first][second] * solve(rest ^ (1 << second))
            answer += term if position % 2 == 1 else -term
            bits ^= 1 << second
        return answer % MOD

    return solve((1 << size) - 1)


def random_alternating(random_source: random.Random, size: int) -> list[list[int]]:
    matrix = [[0] * size for _ in range(size)]
    for row in range(size):
        for column in range(row + 1, size):
            value = random_source.randrange(MOD)
            matrix[row][column] = value
            matrix[column][row] = (-value) % MOD
    return matrix


def write_case(out_dir: Path, index: int, matrix: list[list[int]], answer: int) -> None:
    input_data = f"{len(matrix)}\n" + "".join(
        " ".join(map(str, row)) + "\n" for row in matrix
    )
    (out_dir / f"case_{index:02d}.in").write_text(input_data, encoding="utf-8")
    (out_dir / f"case_{index:02d}.out").write_text(
        f"{answer % MOD}\n", encoding="utf-8"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)

    matrices: list[list[list[int]]] = [
        [],
        [[0, 7], [MOD - 7, 0]],
        [[0, 0, 0, 3], [0, 0, 5, 0], [0, MOD - 5, 0, 0], [MOD - 3, 0, 0, 0]],
        [[0] * 8 for _ in range(8)],
    ]
    random_source = random.Random(2026071304)
    for size in [2, 4, 6, 8, 10, 12]:
        for _ in range(8):
            matrices.append(random_alternating(random_source, size))

    for index, matrix in enumerate(matrices):
        write_case(args.out_dir, index, matrix, pfaffian_brute(matrix))

    size = 600
    block = [[0] * size for _ in range(size)]
    answer = 1
    for row in range(0, size, 2):
        value = random_source.randrange(1, MOD)
        block[row][row + 1] = value
        block[row + 1][row] = (-value) % MOD
        answer = answer * value % MOD
    write_case(args.out_dir, len(matrices), block, answer)


if __name__ == "__main__":
    main()
