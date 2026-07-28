#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


MASK = (1 << 64) - 1


def convolution(first: list[int], second: list[int]) -> list[int]:
    result = [0] * (len(first) + len(second) - 1)
    for first_index, first_value in enumerate(first):
        for second_index, second_value in enumerate(second):
            result[first_index + second_index] += (
                first_value * second_value
            )
            result[first_index + second_index] &= MASK
    return result


def write_cases(
    output_directory: Path,
    name: str,
    cases: list[tuple[list[int], list[int]]],
) -> None:
    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for first, second in cases:
        input_lines.append(f"{len(first)} {len(second)}")
        input_lines.append(" ".join(map(str, first)))
        input_lines.append(" ".join(map(str, second)))
        output_lines.append(" ".join(map(str, convolution(first, second))))
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
            ([0], [0]),
            ([MASK], [MASK]),
            ([0, 1, 1 << 63, MASK], [MASK, 2, 1 << 63]),
        ],
    )
    random_engine = random.Random(arguments.seed)
    cases: list[tuple[list[int], list[int]]] = []
    for _ in range(30):
        first_size = random_engine.randint(1, 180)
        second_size = random_engine.randint(1, 180)
        first = [
            random_engine.getrandbits(64) for _ in range(first_size)
        ]
        second = [
            random_engine.getrandbits(64) for _ in range(second_size)
        ]
        cases.append((first, second))
    write_cases(arguments.out_dir, "random", cases)


if __name__ == "__main__":
    main()
