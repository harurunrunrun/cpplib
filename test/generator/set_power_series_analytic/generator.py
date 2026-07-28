#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


MOD = 998_244_353


def exponential(series: list[int], bit_count: int) -> list[int]:
    result = [0] * len(series)
    result[0] = 1
    for subset in range(1, len(series)):
        degree = subset.bit_count()
        value = 0
        part = subset
        while part:
            value += (
                part.bit_count()
                * series[part]
                * result[subset ^ part]
            )
            part = (part - 1) & subset
        result[subset] = value * pow(degree, MOD - 2, MOD) % MOD
    return result


def logarithm(series: list[int], bit_count: int) -> list[int]:
    result = [0] * len(series)
    for subset in range(1, len(series)):
        degree = subset.bit_count()
        value = degree * series[subset]
        part = (subset - 1) & subset
        while part:
            value -= (
                part.bit_count()
                * result[part]
                * series[subset ^ part]
            )
            part = (part - 1) & subset
        result[subset] = value * pow(degree, MOD - 2, MOD) % MOD
    return result


def write_case(
    out_dir: Path,
    name: str,
    cases: list[tuple[str, int, list[int]]],
) -> None:
    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for operation, bit_count, series in cases:
        input_lines.append(f"{operation} {bit_count}")
        input_lines.append(" ".join(map(str, series)))
        result = (
            exponential(series, bit_count)
            if operation == "E"
            else logarithm(series, bit_count)
        )
        output_lines.append(" ".join(map(str, result)))
    (out_dir / f"{name}.in").write_text(
        "\n".join(input_lines) + "\n", encoding="utf-8"
    )
    (out_dir / f"{name}.out").write_text(
        "\n".join(output_lines) + "\n", encoding="utf-8"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", type=Path, required=True)
    parser.add_argument("--seed", type=int, default=20260728)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)
    write_case(
        args.out_dir,
        "fixed",
        [
            ("E", 0, [0]),
            ("L", 0, [1]),
            ("E", 3, [0, 1, 2, 3, 4, 5, 6, 7]),
            ("L", 3, [1, 1, 2, 3, 4, 5, 6, 7]),
        ],
    )
    random_engine = random.Random(args.seed)
    cases: list[tuple[str, int, list[int]]] = []
    for _ in range(50):
        bit_count = random_engine.randint(0, 8)
        subset_count = 1 << bit_count
        for operation, constant in (("E", 0), ("L", 1)):
            series = [
                random_engine.randrange(MOD)
                for _ in range(subset_count)
            ]
            series[0] = constant
            cases.append((operation, bit_count, series))
    write_case(args.out_dir, "random", cases)


if __name__ == "__main__":
    main()
