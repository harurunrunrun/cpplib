#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path

Point = tuple[int, int]


def distance(first: Point, second: Point) -> int:
    dx = first[0] - second[0]
    dy = first[1] - second[1]
    return dx * dx + dy * dy


def brute_weights(points: list[Point]) -> list[int]:
    if not points:
        return []
    size = len(points)
    infinity = 10**100
    best = [infinity] * size
    used = [False] * size
    best[0] = 0
    result: list[int] = []
    for step in range(size):
        vertex = min((best[index], index) for index in range(size) if not used[index])[1]
        used[vertex] = True
        if step != 0:
            result.append(best[vertex])
        for other in range(size):
            if not used[other]:
                best[other] = min(best[other], distance(points[vertex], points[other]))
    return sorted(result)


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
            weights = brute_weights(points)
            output_file.write(f"{len(weights)}")
            if weights:
                output_file.write(" " + " ".join(map(str, weights)))
            output_file.write("\n")


def write_max_grid(out_dir: Path, index: int) -> None:
    size = 200_000
    width = 447
    with (out_dir / f"case_{index:02d}.in").open(
        "w", encoding="utf-8", newline="\n"
    ) as input_file, (out_dir / f"case_{index:02d}.out").open(
        "w", encoding="utf-8", newline="\n"
    ) as output_file:
        input_file.write(f"1\n{size}\n")
        for index_value in range(size):
            input_file.write(f"{index_value % width} {index_value // width}\n")
        output_file.write(f"{size - 1} " + " ".join(["1"] * (size - 1)) + "\n")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)

    basics = [
        [(0, 0)],
        [(0, 0), (0, 0)],
        [(0, 0), (3, 0), (0, 4)],
        [(0, 0), (1, 0), (2, 0), (3, 0), (4, 0)],
        [(0, 0), (2, 0), (2, 2), (0, 2), (1, 1)],
        [(-10_000, -10_000), (10_000, 10_000), (-10_000, 10_000), (10_000, -10_000)],
    ]
    write_batch(args.out_dir, 0, basics)

    source = random.Random(2026071318)
    random_cases: list[list[Point]] = []
    for _ in range(450):
        size = source.randint(1, 55)
        random_cases.append([
            (source.randint(-50, 50), source.randint(-50, 50))
            for _ in range(size)
        ])
    write_batch(args.out_dir, 1, random_cases)
    write_max_grid(args.out_dir, 2)


if __name__ == "__main__":
    main()
