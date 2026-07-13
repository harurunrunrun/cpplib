#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from functools import lru_cache
from pathlib import Path

MOD = 998244353


def naive_hafnian(matrix: list[list[int]]) -> int:
    size = len(matrix)

    @lru_cache(maxsize=None)
    def solve(mask: int) -> int:
        if mask == 0:
            return 1
        first_flag = mask & -mask
        first = first_flag.bit_length() - 1
        remaining = mask ^ first_flag
        answer = 0
        candidates = remaining
        while candidates:
            second_flag = candidates & -candidates
            candidates ^= second_flag
            second = second_flag.bit_length() - 1
            answer += matrix[first][second] * solve(remaining ^ second_flag)
        return answer % MOD

    return solve((1 << size) - 1)


def write_case(out_dir: Path, index: int, matrix: list[list[int]], answer: int) -> None:
    with (out_dir / f"case_{index:02d}.in").open(
        "w", encoding="utf-8", newline="\n"
    ) as output:
        output.write(f"{len(matrix)}\n")
        output.writelines(" ".join(map(str, row)) + "\n" for row in matrix)
    (out_dir / f"case_{index:02d}.out").write_text(
        f"{answer % MOD}\n", encoding="utf-8"
    )


def random_symmetric(source: random.Random, size: int) -> list[list[int]]:
    matrix = [[0] * size for _ in range(size)]
    for row in range(size):
        for column in range(row + 1, size):
            value = source.randrange(MOD)
            matrix[row][column] = value
            matrix[column][row] = value
    return matrix


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)

    write_case(args.out_dir, 0, [], 1)
    write_case(args.out_dir, 1, [[0, 7], [7, 0]], 7)
    source = random.Random(2026071318)
    index = 2
    for size in range(2, 14, 2):
        for _ in range(5):
            matrix = random_symmetric(source, size)
            write_case(args.out_dir, index, matrix, naive_hafnian(matrix))
            index += 1

    size = 38
    all_one = [[int(row != column) for column in range(size)] for row in range(size)]
    double_factorial = 1
    for value in range(1, size, 2):
        double_factorial = double_factorial * value % MOD
    write_case(args.out_dir, index, all_one, double_factorial)


if __name__ == "__main__":
    main()
