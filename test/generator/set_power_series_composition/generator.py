#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


MOD = 998_244_353


def subset_product(first: list[int], second: list[int]) -> list[int]:
    result = [0] * len(first)
    for subset in range(len(first)):
        part = subset
        while True:
            result[subset] += first[part] * second[subset ^ part]
            if part == 0:
                break
            part = (part - 1) & subset
        result[subset] %= MOD
    return result


def polynomial_composite(
    polynomial: list[int],
    series: list[int],
) -> list[int]:
    result = [0] * len(series)
    power = [0] * len(series)
    power[0] = 1
    for coefficient in polynomial:
        for subset in range(len(series)):
            result[subset] += coefficient * power[subset]
            result[subset] %= MOD
        power = subset_product(power, series)
    return result


def projection(
    series: list[int],
    weights: list[int],
    count: int,
) -> list[int]:
    result: list[int] = []
    power = [0] * len(series)
    power[0] = 1
    for _ in range(count):
        result.append(sum(
            weight * coefficient
            for weight, coefficient in zip(weights, power)
        ) % MOD)
        power = subset_product(power, series)
    return result


def write_cases(
    output_directory: Path,
    name: str,
    cases: list[tuple[str, int, list[int], list[int]]],
) -> None:
    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for operation, count, first, second in cases:
        bit_count = (len(second) - 1).bit_length()
        input_lines.append(f"{operation} {bit_count} {count}")
        input_lines.append(" ".join(map(str, first)))
        input_lines.append(" ".join(map(str, second)))
        if operation == "C":
            answer = polynomial_composite(first, second)
        else:
            answer = projection(first, second, count)
        output_lines.append(" ".join(map(str, answer)))
    (output_directory / f"{name}.in").write_text(
        "\n".join(input_lines) + "\n", encoding="utf-8"
    )
    (output_directory / f"{name}.out").write_text(
        "\n".join(output_lines) + "\n", encoding="utf-8"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    parser.add_argument("--seed", type=int, default=20260728)
    arguments = parser.parse_args()
    arguments.out_dir.mkdir(parents=True, exist_ok=True)

    write_cases(
        arguments.out_dir,
        "fixed",
        [
            ("C", 0, [], [7]),
            ("C", 3, [1, 2, 3], [4]),
            ("C", 4, [1, 2, 3, 4], [5, 1, 2, 3]),
            ("P", 0, [2], [3]),
            ("P", 7, [2, 1, 3, 4], [4, 5, 6, 7]),
        ],
    )

    random_engine = random.Random(arguments.seed)
    cases: list[tuple[str, int, list[int], list[int]]] = []
    for _ in range(60):
        bit_count = random_engine.randint(0, 8)
        subset_count = 1 << bit_count
        series = [
            random_engine.randrange(MOD) for _ in range(subset_count)
        ]
        polynomial_size = random_engine.randint(0, 18)
        polynomial = [
            random_engine.randrange(MOD)
            for _ in range(polynomial_size)
        ]
        cases.append(("C", polynomial_size, polynomial, series))
        output_count = random_engine.randint(0, 30)
        weights = [
            random_engine.randrange(MOD) for _ in range(subset_count)
        ]
        cases.append(("P", output_count, series, weights))
    write_cases(arguments.out_dir, "random", cases)


if __name__ == "__main__":
    main()
