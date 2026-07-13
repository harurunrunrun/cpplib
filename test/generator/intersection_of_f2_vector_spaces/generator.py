#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path

BIT_WIDTH = 32


def canonical_basis(values: set[int] | list[int]) -> list[int]:
    basis = [0] * BIT_WIDTH
    for original in values:
        value = original
        for bit in range(BIT_WIDTH - 1, -1, -1):
            if not value >> bit & 1:
                continue
            if basis[bit]:
                value ^= basis[bit]
            else:
                basis[bit] = value
                break
    for low in range(BIT_WIDTH):
        if not basis[low]:
            continue
        for high in range(low + 1, BIT_WIDTH):
            if basis[high] >> low & 1:
                basis[high] ^= basis[low]
    return [basis[bit] for bit in range(BIT_WIDTH - 1, -1, -1) if basis[bit]]


def span(generators: list[int]) -> set[int]:
    result = {0}
    for value in generators:
        result |= {current ^ value for current in tuple(result)}
    return result


def intersection_brute(first: list[int], second: list[int]) -> list[int]:
    return canonical_basis(span(first) & span(second))


def independent_random_basis(
    random_source: random.Random,
    dimension: int,
    bit_count: int,
) -> list[int]:
    basis: list[int] = []
    while len(basis) < dimension:
        candidate = random_source.randrange(1, 1 << bit_count)
        if len(canonical_basis(basis + [candidate])) > len(basis):
            basis.append(candidate)
    return basis


def write_batch(
    out_dir: Path,
    index: int,
    cases: list[tuple[list[int], list[int], list[int]]],
) -> None:
    with (out_dir / f"case_{index:02d}.in").open(
        "w", encoding="utf-8", newline="\n"
    ) as input_file, (out_dir / f"case_{index:02d}.out").open(
        "w", encoding="utf-8", newline="\n"
    ) as output_file:
        input_file.write(f"{len(cases)}\n")
        for first, second, answer in cases:
            input_file.write(f"{len(first)}")
            if first:
                input_file.write(" " + " ".join(map(str, first)))
            input_file.write("\n")
            input_file.write(f"{len(second)}")
            if second:
                input_file.write(" " + " ".join(map(str, second)))
            input_file.write("\n")
            output_file.write(f"{len(answer)}")
            if answer:
                output_file.write(" " + " ".join(map(str, answer)))
            output_file.write("\n")


def write_max_batch(out_dir: Path, index: int) -> None:
    test_count = 100_000
    basis = [1 << bit for bit in range(30)]
    answer = list(reversed(basis))
    with (out_dir / f"case_{index:02d}.in").open(
        "w", encoding="utf-8", newline="\n"
    ) as input_file, (out_dir / f"case_{index:02d}.out").open(
        "w", encoding="utf-8", newline="\n"
    ) as output_file:
        input_file.write(f"{test_count}\n")
        input_file.write("30 " + " ".join(map(str, basis)) + "\n")
        input_file.write("30 " + " ".join(map(str, reversed(basis))) + "\n")
        output_file.write("30 " + " ".join(map(str, answer)) + "\n")
        for _ in range(test_count - 1):
            input_file.write("0\n0\n")
            output_file.write("0\n")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)

    basic = [
        ([], [], []),
        ([1, 2], [2, 4], [2]),
        ([3, 5], [6, 5], intersection_brute([3, 5], [6, 5])),
        ([1, 2, 4, 8], [15], [15]),
    ]
    write_batch(args.out_dir, 0, basic)

    random_source = random.Random(2026071313)
    random_cases: list[tuple[list[int], list[int], list[int]]] = []
    for _ in range(80):
        bit_count = random_source.randint(1, 10)
        first_dimension = random_source.randint(0, min(8, bit_count))
        second_dimension = random_source.randint(0, min(8, bit_count))
        first = independent_random_basis(
            random_source, first_dimension, bit_count
        )
        second = independent_random_basis(
            random_source, second_dimension, bit_count
        )
        random_cases.append((first, second, intersection_brute(first, second)))
    write_batch(args.out_dir, 1, random_cases)
    write_max_batch(args.out_dir, 2)


if __name__ == "__main__":
    main()
