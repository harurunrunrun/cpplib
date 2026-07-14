#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path

Point = tuple[int, int]


def write_batch(out_dir: Path, index: int, cases: list[list[Point]]) -> None:
    with (out_dir / f"case_{index:02d}.in").open(
        "w", encoding="utf-8", newline="\n"
    ) as input_file, (out_dir / f"case_{index:02d}.out").open(
        "w", encoding="utf-8", newline="\n"
    ) as output_file:
        input_file.write(f"{len(cases)}\n")
        for points in cases:
            input_file.write(f"{len(points)}\n")
            input_file.writelines(f"{x} {y}\n" for x, y in points)
            output_file.write("OK\n")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)

    basics: list[list[Point]] = [
        [],
        [(0, 0)],
        [(0, 0), (0, 0)],
        [(-2, 0), (2, 0)],
        [(-4, 0), (-1, 0), (3, 0), (7, 0), (-1, 0)],
        [(0, 0), (6, 0), (0, 8)],
        [(0, 0), (4, 0), (4, 4), (0, 4)],
        [(0, 0), (4, 0), (4, 4), (0, 4), (2, 2)],
        [
            (5, 0), (3, 4), (0, 5), (-3, 4), (-5, 0),
            (-3, -4), (0, -5), (3, -4),
        ],
    ]
    write_batch(args.out_dir, 0, basics)

    source = random.Random(2026071502)
    random_cases: list[list[Point]] = []
    for _ in range(280):
        size = source.randint(0, 45)
        random_cases.append([
            (source.randint(-15, 15), source.randint(-15, 15))
            for _ in range(size)
        ])
    write_batch(args.out_dir, 1, random_cases)

    grid = [(x, y) for y in range(45) for x in range(45)]
    collinear = [(index, 3 * index + 1) for index in range(4000)]
    write_batch(args.out_dir, 2, [grid, collinear])


if __name__ == "__main__":
    main()
