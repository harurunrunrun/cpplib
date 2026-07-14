#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def solve(distances: list[list[int]]) -> list[tuple[int, int]]:
    town_count = len(distances)
    return [
        (left, right)
        for left in range(town_count)
        for right in range(left + 1, town_count)
        if all(
            middle in (left, right)
            or distances[left][right] != distances[left][middle] + distances[middle][right]
            for middle in range(town_count)
        )
    ]


def all_pairs_shortest(weights: list[list[int]]) -> list[list[int]]:
    result = [row[:] for row in weights]
    for middle in range(len(result)):
        for left in range(len(result)):
            for right in range(len(result)):
                result[left][right] = min(
                    result[left][right],
                    result[left][middle] + result[middle][right],
                )
    return result


def write_case(out_dir: Path, name: str, tests: list[list[list[int]]]) -> None:
    input_lines = [str(len(tests))]
    output_lines = []
    for test_index, distances in enumerate(tests):
        input_lines.append(str(len(distances)))
        input_lines.extend(" ".join(map(str, row)) for row in distances)
        if test_index:
            output_lines.append("")
        output_lines.extend(
            f"{left + 1} {right + 1}" for left, right in solve(distances)
        )
    (out_dir / f"{name}.in").write_text("\n".join(input_lines) + "\n", encoding="ascii")
    (out_dir / f"{name}.out").write_text("\n".join(output_lines) + "\n", encoding="ascii")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    write_case(out_dir, "basic", [
        [[0]],
        [[0, 1, 2], [1, 0, 3], [2, 3, 0]],
        [[0, 2, 5], [2, 0, 3], [5, 3, 0]],
    ])

    source = random.Random(145)
    tests = []
    infinity = 10**9
    for _ in range(35):
        town_count = source.randint(2, 11)
        weights = [[0 if left == right else infinity for right in range(town_count)] for left in range(town_count)]
        for left in range(town_count):
            for right in range(left + 1, town_count):
                if source.random() < 0.55 or right == left + 1:
                    weight = source.randint(1, 30)
                    weights[left][right] = weights[right][left] = weight
        tests.append(all_pairs_shortest(weights))
    write_case(out_dir, "random", tests)

    maximum = [[abs(left - right) for right in range(200)] for left in range(200)]
    write_case(out_dir, "maximum", [maximum])


if __name__ == "__main__":
    main()
