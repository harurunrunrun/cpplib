#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import math
import random
from pathlib import Path

Query = tuple[int, int, int, int, int, int, int, int]


def answer(query: Query) -> int:
    ox, oy, dx, dy, aperture, px, py, include = query
    rx, ry = px - ox, py - oy
    if rx == 0 and ry == 0:
        return 1
    if aperture == 360:
        return 1
    angle = abs(math.atan2(dx * ry - dy * rx, dx * rx + dy * ry))
    half = math.radians(aperture) / 2.0
    if include:
        return int(angle <= half + 1e-12)
    return int(angle < half - 1e-12)


def write_case(
    output_directory: Path,
    index: int,
    queries: list[Query],
) -> None:
    with (output_directory / f"case_{index:02d}.in").open(
        "w", encoding="utf-8", newline="\n"
    ) as input_file, (output_directory / f"case_{index:02d}.out").open(
        "w", encoding="utf-8", newline="\n"
    ) as output_file:
        input_file.write(f"{len(queries)}\n")
        input_file.writelines(" ".join(map(str, query)) + "\n"
                              for query in queries)
        output_file.writelines(f"{answer(query)}\n" for query in queries)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    arguments = parser.parse_args()
    arguments.out_dir.mkdir(parents=True, exist_ok=True)

    basic: list[Query] = [
        (0, 0, 1, 0, 90, 1, 0, 1),
        (0, 0, 1, 0, 90, 1, 1, 1),
        (0, 0, 1, 0, 90, 1, 1, 0),
        (0, 0, 1, 0, 90, 0, 1, 1),
        (0, 0, 1, 0, 360, -1, 0, 0),
        (0, 0, 1, 0, 0, 2, 0, 1),
        (0, 0, 1, 0, 0, 2, 0, 0),
        (5, -2, -3, 4, 0, 5, -2, 0),
    ]
    write_case(arguments.out_dir, 0, basic)

    source = random.Random(2026071506)
    queries: list[Query] = []
    for _ in range(10000):
        dx = dy = 0
        while dx == 0 and dy == 0:
            dx, dy = source.randint(-20, 20), source.randint(-20, 20)
        query = (
            source.randint(-20, 20),
            source.randint(-20, 20),
            dx,
            dy,
            source.randint(0, 360),
            source.randint(-50, 50),
            source.randint(-50, 50),
            source.randint(0, 1),
        )
        queries.append(query)
    write_case(arguments.out_dir, 1, queries)

    maximum: list[Query] = []
    for index in range(200000):
        y = index % 7 - 3
        maximum.append((0, 0, 1, 0, 90, 1, y, index & 1))
    write_case(arguments.out_dir, 2, maximum)


if __name__ == "__main__":
    main()
