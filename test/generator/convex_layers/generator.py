#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path

Point = tuple[int, int]


def cross(first: Point, second: Point, third: Point) -> int:
    return ((second[0] - first[0]) * (third[1] - first[1])
            - (second[1] - first[1]) * (third[0] - first[0]))


def boundary(points: set[Point]) -> set[Point]:
    ordered = sorted(points)
    if len(ordered) <= 2:
        return set(ordered)
    lower: list[Point] = []
    for point in ordered:
        while len(lower) >= 2 and cross(lower[-2], lower[-1], point) < 0:
            lower.pop()
        lower.append(point)
    upper: list[Point] = []
    for point in reversed(ordered):
        while len(upper) >= 2 and cross(upper[-2], upper[-1], point) < 0:
            upper.pop()
        upper.append(point)
    return set(lower + upper)


def brute(points: list[Point]) -> list[int]:
    remaining = set(points)
    layer_by_point: dict[Point, int] = {}
    layer = 1
    while remaining:
        removed = boundary(remaining)
        for point in removed:
            layer_by_point[point] = layer
        remaining -= removed
        layer += 1
    return [layer_by_point[point] for point in points]


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
            output_file.write(" ".join(map(str, brute(points))) + "\n")


def write_max(out_dir: Path, index: int) -> None:
    size = 200_000
    with (out_dir / f"case_{index:02d}.in").open(
        "w", encoding="utf-8", newline="\n"
    ) as input_file, (out_dir / f"case_{index:02d}.out").open(
        "w", encoding="utf-8", newline="\n"
    ) as output_file:
        input_file.write(f"1\n{size}\n")
        input_file.writelines(f"{value} 0\n" for value in range(size))
        output_file.write(" ".join(["1"] * size) + "\n")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)

    basics = [
        [(0, 0)],
        [(0, 0), (1, 0), (2, 0), (3, 0)],
        [(0, 0), (2, 0), (2, 2), (0, 2), (1, 1)],
        [(0, 0), (4, 0), (4, 4), (0, 4), (1, 1), (3, 1), (3, 3), (1, 3), (2, 2)],
        [(0, 0), (0, 0), (2, 0), (1, 1)],
    ]
    write_batch(args.out_dir, 0, basics)

    source = random.Random(2026071317)
    random_cases: list[list[Point]] = []
    for _ in range(350):
        size = source.randint(1, 60)
        coordinates: set[Point] = set()
        while len(coordinates) < size:
            coordinates.add((source.randint(-25, 25), source.randint(-25, 25)))
        random_cases.append(list(coordinates))
    write_batch(args.out_dir, 1, random_cases)
    write_max(args.out_dir, 2)


if __name__ == "__main__":
    main()
