#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path

Point = tuple[int, int]


def brute(points: list[Point]) -> tuple[int, int]:
    answer = (-1, -1)
    best: int | None = None
    for first in range(len(points)):
        for second in range(first + 1, len(points)):
            dx = points[first][0] - points[second][0]
            dy = points[first][1] - points[second][1]
            distance = dx * dx + dy * dy
            if best is None or distance < best:
                best = distance
                answer = (first, second)
    return answer


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
            first, second = brute(points)
            output_file.write(f"{first} {second}\n")


def write_max(out_dir: Path, index: int) -> None:
    size = 500_000
    with (out_dir / f"case_{index:02d}.in").open(
        "w", encoding="utf-8", newline="\n"
    ) as input_file, (out_dir / f"case_{index:02d}.out").open(
        "w", encoding="utf-8", newline="\n"
    ) as output_file:
        input_file.write(f"1\n{size}\n")
        input_file.writelines(f"{value} 0\n" for value in range(size))
        output_file.write("0 1\n")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)

    basics = [
        [(0, 0), (1, 0)],
        [(0, 0), (0, 0), (0, 0)],
        [(0, 0), (1, 1), (1, -1), (-1, 1), (-1, -1)],
        [(-1_000_000_000, -1_000_000_000), (1_000_000_000, 1_000_000_000)],
        [(5, 0), (3, 0), (4, 0), (0, 0)],
    ]
    write_batch(args.out_dir, 0, basics)

    source = random.Random(2026071316)
    random_cases: list[list[Point]] = []
    for _ in range(500):
        size = source.randint(2, 45)
        random_cases.append([
            (source.randint(-30, 30), source.randint(-30, 30))
            for _ in range(size)
        ])
    write_batch(args.out_dir, 1, random_cases)
    write_max(args.out_dir, 2)


if __name__ == "__main__":
    main()
