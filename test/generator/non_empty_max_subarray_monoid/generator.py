#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def aggregate(values: list[int]) -> tuple[int, int, int, int, int]:
    if not values:
        return 1, 0, 0, 0, 0
    running = 0
    prefix = values[0]
    for value in values:
        running += value
        prefix = max(prefix, running)
    total = running
    running = 0
    suffix = values[-1]
    for value in reversed(values):
        running += value
        suffix = max(suffix, running)
    current = best = values[0]
    for value in values[1:]:
        current = max(value, current + value)
        best = max(best, current)
    return 0, total, prefix, suffix, best


def write_case(
    output_directory: Path,
    index: int,
    cases: list[tuple[str, list[int]]],
) -> None:
    name = f"case_{index:02d}"
    input_lines = [str(len(cases))]
    output_lines = []
    for type_name, values in cases:
        input_lines.append(f"{type_name} {len(values)}")
        input_lines.append(" ".join(map(str, values)))
        output_lines.append(" ".join(map(str, aggregate(values))))
    (output_directory / f"{name}.in").write_text(
        "\n".join(input_lines) + "\n",
        encoding="utf-8",
    )
    (output_directory / f"{name}.out").write_text(
        "\n".join(output_lines) + "\n",
        encoding="utf-8",
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    arguments = parser.parse_args()
    output_directory = Path(arguments.out_dir)
    output_directory.mkdir(parents=True, exist_ok=True)

    int32_min = -(1 << 31)
    int32_max = (1 << 31) - 1
    int64_min = -(1 << 63)
    int64_max = (1 << 63) - 1
    cases: list[tuple[str, list[int]]] = [
        ("I32", []),
        ("I32", [-8, -3, -5, -2, -9]),
        ("I32", [0]),
        ("I32", [0, -4, 0]),
        ("I32", [4, -2, 3, -10, 8, 1]),
        ("I32", [int32_min]),
        ("I32", [int32_max]),
        ("I32", [int32_max - 100, -40]),
        ("I32", [int32_min + 100, 40]),
        ("I64", [int64_min]),
        ("I64", [int64_max]),
        ("I64", [int64_max - 1000, -400]),
        ("I64", [int64_min + 1000, 400]),
    ]
    rng = random.Random(20260714)
    for _ in range(600):
        type_name = "I32" if rng.randrange(2) == 0 else "I64"
        size = rng.randrange(0, 81)
        mode = rng.randrange(4)
        if mode == 0:
            values = [-rng.randrange(1, 1000001) for _ in range(size)]
        elif mode == 1:
            values = [0 for _ in range(size)]
        else:
            values = [rng.randrange(-1000000, 1000001) for _ in range(size)]
        cases.append((type_name, values))
    write_case(output_directory, 0, cases)

    maximum_values = [
        ((index * 1009 + 17) % 2001) - 1000
        for index in range(100000)
    ]
    write_case(output_directory, 1, [("I64", maximum_values)])


if __name__ == "__main__":
    main()
