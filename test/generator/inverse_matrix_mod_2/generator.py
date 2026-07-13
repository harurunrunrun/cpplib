#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def bits_to_string(value: int, size: int) -> str:
    return "".join("1" if value >> column & 1 else "0" for column in range(size))


def inverse_gaussian(matrix: list[int], size: int) -> list[int] | None:
    rows = [matrix[row] | (1 << (size + row)) for row in range(size)]
    for column in range(size):
        pivot = next(
            (row for row in range(column, size) if rows[row] >> column & 1),
            None,
        )
        if pivot is None:
            return None
        rows[column], rows[pivot] = rows[pivot], rows[column]
        for row in range(size):
            if row != column and rows[row] >> column & 1:
                rows[row] ^= rows[column]
    mask = (1 << size) - 1
    return [(row >> size) & mask for row in rows]


def constructed_inverse_pair(
    random_source: random.Random,
    size: int,
    operation_count: int,
) -> tuple[list[int], list[int]]:
    matrix = [1 << row for row in range(size)]
    inverse = matrix.copy()
    if size <= 1:
        return matrix, inverse
    for _ in range(operation_count):
        first = random_source.randrange(size)
        second = random_source.randrange(size - 1)
        if second >= first:
            second += 1
        if random_source.randrange(4) == 0:
            matrix[first], matrix[second] = matrix[second], matrix[first]
            first_bit = 1 << first
            second_bit = 1 << second
            for row in range(size):
                left = bool(inverse[row] & first_bit)
                right = bool(inverse[row] & second_bit)
                if left != right:
                    inverse[row] ^= first_bit | second_bit
        else:
            matrix[first] ^= matrix[second]
            first_bit = 1 << first
            second_bit = 1 << second
            for row in range(size):
                if inverse[row] & first_bit:
                    inverse[row] ^= second_bit
    return matrix, inverse


def write_case(
    out_dir: Path,
    index: int,
    matrix: list[int],
    inverse: list[int] | None = None,
) -> None:
    size = len(matrix)
    if inverse is None:
        inverse = inverse_gaussian(matrix, size)
    with (out_dir / f"case_{index:02d}.in").open(
        "w", encoding="utf-8", newline="\n"
    ) as output:
        output.write(f"{size}\n")
        output.writelines(bits_to_string(row, size) + "\n" for row in matrix)
    with (out_dir / f"case_{index:02d}.out").open(
        "w", encoding="utf-8", newline="\n"
    ) as output:
        if inverse is None:
            output.write("-1\n")
        else:
            output.writelines(bits_to_string(row, size) + "\n" for row in inverse)


def write_max_identity(out_dir: Path, index: int) -> None:
    size = 4096
    with (out_dir / f"case_{index:02d}.in").open(
        "w", encoding="utf-8", newline="\n"
    ) as input_file, (out_dir / f"case_{index:02d}.out").open(
        "w", encoding="utf-8", newline="\n"
    ) as output_file:
        input_file.write(f"{size}\n")
        for row in range(size):
            line = "0" * row + "1" + "0" * (size - row - 1)
            input_file.write(line + "\n")
            output_file.write(line + "\n")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)

    index = 0
    for value in range(16):
        matrix = [value & 3, value >> 2]
        write_case(args.out_dir, index, matrix)
        index += 1

    random_source = random.Random(2026071311)
    for size in range(1, 13):
        for _ in range(4):
            matrix, inverse = constructed_inverse_pair(
                random_source, size, max(1, 8 * size)
            )
            write_case(args.out_dir, index, matrix, inverse)
            assert inverse_gaussian(matrix, size) == inverse
            index += 1
        random_matrix = [random_source.randrange(1 << size) for _ in range(size)]
        write_case(args.out_dir, index, random_matrix)
        index += 1

    matrix, inverse = constructed_inverse_pair(random_source, 256, 8192)
    write_case(args.out_dir, index, matrix, inverse)
    index += 1
    write_max_identity(args.out_dir, index)


if __name__ == "__main__":
    main()
