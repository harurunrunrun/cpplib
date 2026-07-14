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
        [(4, -7)],
        [(4, -7), (4, -7), (4, -7)],
        [(0, 0), (10, 0)],
        [(0, 0), (2, 0), (4, 0), (2, 0), (-2, 0)],
        [(0, 0), (5, 0), (0, 4)],
        [(0, 0), (4, 0), (4, 4), (0, 4)],
        [(0, 0), (4, 0), (4, 4), (0, 4), (2, 2)],
        [
            (5, 0), (3, 4), (0, 5), (-3, 4), (-5, 0),
            (-3, -4), (0, -5), (3, -4), (5, 0),
        ],
        [
            (-1_000_000, -1_000_000),
            (1_000_000, -1_000_000),
            (1_000_000, 1_000_000),
            (-1_000_000, 1_000_000),
            (1, 2),
        ],
    ]
    write_batch(args.out_dir, 0, basics)

    source = random.Random(2026071501)
    random_cases: list[list[Point]] = []
    for _ in range(350):
        size = source.randint(0, 55)
        random_cases.append([
            (source.randint(-20, 20), source.randint(-20, 20))
            for _ in range(size)
        ])
    write_batch(args.out_dir, 1, random_cases)

    grid = [(x, y) for y in range(70) for x in range(70)]
    duplicates = [(index % 31, (index * 17) % 43) for index in range(5000)]
    write_batch(args.out_dir, 2, [grid, duplicates])


if __name__ == "__main__":
    main()
