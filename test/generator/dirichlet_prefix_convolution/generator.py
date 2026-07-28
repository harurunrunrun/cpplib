#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


MOD = 998_244_353


def quotient_values(maximum: int) -> list[int]:
    values: set[int] = set()
    left = 1
    while left <= maximum:
        value = maximum // left
        values.add(value)
        left = maximum // value + 1
    return sorted(values)


def prefix_at(values: list[int], points: list[int]) -> list[int]:
    prefix = [0]
    for value in values[1:]:
        prefix.append((prefix[-1] + value) % MOD)
    return [prefix[point] for point in points]


def convolution(first: list[int], second: list[int]) -> list[int]:
    maximum = len(first) - 1
    result = [0] * (maximum + 1)
    for divisor in range(1, maximum + 1):
        for quotient in range(1, maximum // divisor + 1):
            result[divisor * quotient] += (
                first[divisor] * second[quotient]
            )
            result[divisor * quotient] %= MOD
    return result


def inverse(values: list[int]) -> list[int]:
    maximum = len(values) - 1
    result = [0] * (maximum + 1)
    result[1] = pow(values[1], MOD - 2, MOD)
    for number in range(2, maximum + 1):
        total = 0
        for divisor in range(1, number):
            if number % divisor == 0:
                total += values[number // divisor] * result[divisor]
        result[number] = -total * result[1] % MOD
    return result


def write_cases(
    output_directory: Path,
    name: str,
    cases: list[tuple[str, list[int], list[int] | None]],
) -> None:
    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for operation, first, second in cases:
        maximum = len(first) - 1
        points = quotient_values(maximum)
        first_prefix = prefix_at(first, points)
        input_lines.append(f"{operation} {maximum}")
        input_lines.append(" ".join(map(str, first_prefix)))
        if operation == "C":
            assert second is not None
            second_prefix = prefix_at(second, points)
            input_lines.append(" ".join(map(str, second_prefix)))
            result = convolution(first, second)
        else:
            result = inverse(first)
        output_lines.append(" ".join(map(str, prefix_at(result, points))))
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
            ("C", [0, 2], [0, 3]),
            ("I", [0, 5], None),
            (
                "C",
                [0, 1, 2, 3, 4, 5, 6],
                [0, 6, 5, 4, 3, 2, 1],
            ),
            ("I", [0, 7, 1, 4, 1, 5, 9], None),
        ],
    )

    random_engine = random.Random(arguments.seed)
    cases: list[tuple[str, list[int], list[int] | None]] = []
    for _ in range(80):
        maximum = random_engine.randint(1, 120)
        first = [0] + [
            random_engine.randrange(MOD) for _ in range(maximum)
        ]
        if first[1] == 0:
            first[1] = 1
        second = [0] + [
            random_engine.randrange(MOD) for _ in range(maximum)
        ]
        cases.append(("C", first, second))
        cases.append(("I", first, None))
    write_cases(arguments.out_dir, "random", cases)


if __name__ == "__main__":
    main()
