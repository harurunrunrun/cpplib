#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path

Point = tuple[int, int]


def furthest_pair_brute(points: list[Point]) -> tuple[int, int]:
    best_distance = -1
    best_pair = (-1, -1)
    for first in range(len(points)):
        for second in range(first + 1, len(points)):
            dx = points[first][0] - points[second][0]
            dy = points[first][1] - points[second][1]
            distance = dx * dx + dy * dy
            if distance > best_distance:
                best_distance = distance
                best_pair = (first, second)
    return best_pair


def write_batch(
    out_dir: Path,
    index: int,
    cases: list[tuple[list[Point], tuple[int, int]]],
) -> None:
    with (out_dir / f"case_{index:02d}.in").open(
        "w", encoding="utf-8", newline="\n"
    ) as input_file, (out_dir / f"case_{index:02d}.out").open(
        "w", encoding="utf-8", newline="\n"
    ) as output_file:
        input_file.write(f"{len(cases)}\n")
        for points, answer in cases:
            input_file.write(f"{len(points)}\n")
            input_file.writelines(f"{x} {y}\n" for x, y in points)
            output_file.write(f"{answer[0]} {answer[1]}\n")


def write_max_points(out_dir: Path, index: int) -> None:
    size = 500_000
    with (out_dir / f"case_{index:02d}.in").open(
        "w", encoding="utf-8", newline="\n"
    ) as input_file, (out_dir / f"case_{index:02d}.out").open(
        "w", encoding="utf-8", newline="\n"
    ) as output_file:
        input_file.write("1\n")
        input_file.write(f"{size}\n")
        for index_value in range(size):
            input_file.write(f"{index_value - 250000} 0\n")
        output_file.write(f"0 {size - 1}\n")


def write_max_tests(out_dir: Path, index: int) -> None:
    test_count = 100_000
    with (out_dir / f"case_{index:02d}.in").open(
        "w", encoding="utf-8", newline="\n"
    ) as input_file, (out_dir / f"case_{index:02d}.out").open(
        "w", encoding="utf-8", newline="\n"
    ) as output_file:
        input_file.write(f"{test_count}\n")
        for case in range(test_count):
            input_file.write("2\n")
            input_file.write(f"{case} {-case}\n")
            input_file.write(f"{case + 1} {-case + 2}\n")
            output_file.write("0 1\n")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)

    point_sets: list[list[Point]] = [
        [(0, 0), (0, 0)],
        [(0, 0), (1, 0), (1, 1), (0, 1)],
        [(5, 5), (5, 5), (-5, -5), (-5, -5)],
        [(-1_000_000_000, -1_000_000_000), (1_000_000_000, 1_000_000_000)],
        [(0, 0), (1, 1), (2, 2), (-3, -3), (2, 2)],
        [(0, 0), (3, 0), (0, 4), (3, 4), (1, 2)],
        [
            (0, 0),
            (-18767, -43052),
            (-21874, -41502),
            (-100000, 0),
            (-280, 6810),
            (-189, 4855),
        ],
    ]
    basic = [(points, furthest_pair_brute(points)) for points in point_sets]
    write_batch(args.out_dir, 0, basic)

    random_source = random.Random(2026071315)
    random_cases: list[tuple[list[Point], tuple[int, int]]] = []
    for _ in range(300):
        size = random_source.randint(2, 35)
        points = [
            (random_source.randint(-20, 20), random_source.randint(-20, 20))
            for _ in range(size)
        ]
        random_cases.append((points, furthest_pair_brute(points)))
    write_batch(args.out_dir, 1, random_cases)
    write_max_points(args.out_dir, 2)
    write_max_tests(args.out_dir, 3)


if __name__ == "__main__":
    main()
