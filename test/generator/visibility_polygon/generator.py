#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import math
import random
from pathlib import Path

Point = tuple[float, float]
Case = tuple[int, Point, list[Point]]


def write_cases(output_directory: Path, index: int, cases: list[Case]) -> None:
    with (output_directory / f"case_{index:02d}.in").open(
        "w", encoding="utf-8", newline="\n"
    ) as input_file, (output_directory / f"case_{index:02d}.out").open(
        "w", encoding="utf-8", newline="\n"
    ) as output_file:
        input_file.write(f"{len(cases)}\n")
        for valid, observer, polygon in cases:
            input_file.write(
                f"{valid} {len(polygon)} {observer[0]:.18g} {observer[1]:.18g}\n"
            )
            input_file.writelines(f"{x:.18g} {y:.18g}\n" for x, y in polygon)
            output_file.write("OK\n" if valid else "ERR\n")


def radial_polygon(
    source: random.Random,
    size: int,
    offset: float,
) -> list[Point]:
    result: list[Point] = []
    for index in range(size):
        angle = offset + 2.0 * math.pi * index / size
        angle += source.uniform(-0.2, 0.2) * math.pi / size
        radius = source.uniform(20.0, 100.0)
        result.append((radius * math.cos(angle), radius * math.sin(angle)))
    return result


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    arguments = parser.parse_args()
    arguments.out_dir.mkdir(parents=True, exist_ok=True)

    near_seam = [
        (-10.0, -1e-7),
        (-5.0, -5.0),
        (5.0, -5.0),
        (5.0, 5.0),
        (-10.0, 1e-7),
        (-12.0, 0.0),
    ]
    close_angles = [
        (10.0, 0.0),
        (10.0, 1e-8),
        (0.0, 10.0),
        (-10.0, 0.0),
        (0.0, -10.0),
    ]
    cases: list[Case] = [
        (1, (0.0, 0.0), [(-5.0, -5.0), (5.0, -5.0),
                          (5.0, 5.0), (-5.0, 5.0)]),
        (1, (0.0, 0.0), [(-5.0, 5.0), (5.0, 5.0),
                          (5.0, -5.0), (-5.0, -5.0)]),
        (1, (0.0, 0.0), [(-5.0, -5.0), (5.0, -5.0),
                          (5.0, 5.0), (1.0, 5.0), (1.0, 1.0),
                          (-1.0, 1.0), (-1.0, 5.0), (-5.0, 5.0)]),
        (1, (0.0, 0.0), near_seam),
        (1, (0.0, 0.0), close_angles),
        (1, (0.0, 0.0), [(-5.0, -5.0), (5.0, -5.0),
                          (5.0, 5.0), (4.0, 5.0), (2.0, 2.0),
                          (0.0, 5.0), (-5.0, 5.0)]),
        (1, (0.0, 0.0), [(-5.0, -5.0), (5.0, -5.0),
                          (5.0, 5.0), (4.0, 4.0), (2.0, 5.0),
                          (-5.0, 5.0)]),
        (1, (100.0, -200.0), [(95.0, -205.0), (105.0, -205.0),
                                (105.0, -195.0), (101.0, -195.0),
                                (101.0, -199.0), (99.0, -199.0),
                                (99.0, -195.0), (95.0, -195.0)]),
        (0, (5.0, 0.0), [(-5.0, -5.0), (5.0, -5.0),
                          (5.0, 5.0), (-5.0, 5.0)]),
        (0, (6.0, 0.0), [(-5.0, -5.0), (5.0, -5.0),
                          (5.0, 5.0), (-5.0, 5.0)]),
        (0, (0.0, 0.0), [(0.0, 0.0), (3.0, 3.0),
                          (0.0, 3.0), (3.0, 0.0)]),
        (0, (1.5, 1.0), [(0.0, 0.0), (4.0, 0.0),
                          (1.0, 0.0), (4.0, 4.0), (0.0, 4.0)]),
    ]
    source = random.Random(2026071507)
    for _ in range(120):
        size = source.randint(5, 18)
        cases.append((
            1,
            (0.0, 0.0),
            radial_polygon(source, size, source.random()),
        ))
    for _ in range(80):
        size = source.randint(6, 24)
        polygon = radial_polygon(source, size, source.random())
        anchor = polygon[source.randrange(size)]
        observer = (0.4 * anchor[0], 0.4 * anchor[1])
        cases.append((1, observer, polygon))
    write_cases(arguments.out_dir, 0, cases)

    size = 800
    maximum = [
        (
            (1000.0 + 200.0 * math.sin(index * 1.7))
            * math.cos(2.0 * math.pi * index / size),
            (1000.0 + 200.0 * math.sin(index * 1.7))
            * math.sin(2.0 * math.pi * index / size),
        )
        for index in range(size)
    ]
    write_cases(arguments.out_dir, 1, [(1, (0.0, 0.0), maximum)])

    size = 20_000
    radius = 1_000_000.0
    regular = [
        (
            radius * math.cos(2.0 * math.pi * index / size),
            radius * math.sin(2.0 * math.pi * index / size),
        )
        for index in range(size)
    ]
    write_cases(arguments.out_dir, 2, [(2, (0.0, 0.0), regular)])

    star_size = 5_001
    circle = [
        (
            10_000.0 * math.cos(2.0 * math.pi * index / star_size),
            10_000.0 * math.sin(2.0 * math.pi * index / star_size),
        )
        for index in range(star_size)
    ]
    star = [circle[(2 * index) % star_size] for index in range(star_size)]
    write_cases(arguments.out_dir, 3, [(0, (0.0, 0.0), star)])


if __name__ == "__main__":
    main()
