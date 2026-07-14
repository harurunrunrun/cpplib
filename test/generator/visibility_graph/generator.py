#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import math
import random
from pathlib import Path

Point = tuple[int, int]


def cross(first: Point, second: Point, third: Point) -> int:
    return (
        (second[0] - first[0]) * (third[1] - first[1])
        - (second[1] - first[1]) * (third[0] - first[0])
    )


def on_segment(first: Point, second: Point, point: Point) -> bool:
    return (
        cross(first, second, point) == 0
        and min(first[0], second[0]) <= point[0] <= max(first[0], second[0])
        and min(first[1], second[1]) <= point[1] <= max(first[1], second[1])
    )


def intersects(a: Point, b: Point, c: Point, d: Point) -> bool:
    if on_segment(a, b, c) or on_segment(a, b, d):
        return True
    if on_segment(c, d, a) or on_segment(c, d, b):
        return True
    return (
        (cross(a, b, c) > 0) != (cross(a, b, d) > 0)
        and (cross(c, d, a) > 0) != (cross(c, d, b) > 0)
    )


def contains_midpoint(polygon: list[Point], first: Point, second: Point) -> bool:
    x = (first[0] + second[0]) / 2.0
    y = (first[1] + second[1]) / 2.0
    inside = False
    for index, a in enumerate(polygon):
        b = polygon[(index + 1) % len(polygon)]
        if abs((b[0] - a[0]) * (y - a[1])
               - (b[1] - a[1]) * (x - a[0])) < 1e-12:
            if min(a[0], b[0]) <= x <= max(a[0], b[0]) and (
                min(a[1], b[1]) <= y <= max(a[1], b[1])
            ):
                return True
        if (a[1] > y) != (b[1] > y):
            boundary_x = a[0] + (b[0] - a[0]) * (y - a[1]) / (b[1] - a[1])
            if x < boundary_x:
                inside = not inside
    return inside


def visible_edges(polygon: list[Point]) -> list[tuple[int, int]]:
    size = len(polygon)
    result: list[tuple[int, int]] = []
    for first in range(size):
        for second in range(first + 1, size):
            adjacent = second == first + 1 or (first == 0 and second + 1 == size)
            visible = adjacent
            if not adjacent:
                visible = not any(
                    vertex not in (first, second)
                    and on_segment(polygon[first], polygon[second], polygon[vertex])
                    for vertex in range(size)
                )
                for edge in range(size):
                    if not visible:
                        break
                    following = (edge + 1) % size
                    if first in (edge, following) or second in (edge, following):
                        continue
                    if intersects(
                        polygon[first], polygon[second],
                        polygon[edge], polygon[following],
                    ):
                        visible = False
                if visible:
                    visible = contains_midpoint(
                        polygon, polygon[first], polygon[second]
                    )
            if visible:
                result.append((first, second))
    return result


def write_cases(
    output_directory: Path,
    index: int,
    cases: list[list[Point]],
) -> None:
    with (output_directory / f"case_{index:02d}.in").open(
        "w", encoding="utf-8", newline="\n"
    ) as input_file, (output_directory / f"case_{index:02d}.out").open(
        "w", encoding="utf-8", newline="\n"
    ) as output_file:
        input_file.write(f"{len(cases)}\n")
        for polygon in cases:
            input_file.write(f"{len(polygon)}\n")
            input_file.writelines(f"{x} {y}\n" for x, y in polygon)
            edges = visible_edges(polygon)
            output_file.write(f"{len(edges)}\n")
            output_file.writelines(f"{first} {second}\n"
                                   for first, second in edges)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    arguments = parser.parse_args()
    arguments.out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[list[Point]] = [
        [(0, 0), (10, 0), (10, 10), (0, 10)],
        [(0, 10), (10, 10), (10, 0), (0, 0)],
        [(0, 0), (10, 0), (10, 10), (7, 10),
         (7, 4), (3, 4), (3, 10), (0, 10)],
        [(0, 0), (8, 0), (8, 8), (4, 4), (0, 8)],
    ]
    source = random.Random(2026071508)
    for _ in range(100):
        width = source.randint(20, 100)
        height = source.randint(20, 100)
        first_x = source.randint(2, width // 2 - 1)
        second_x = source.randint(width // 2 + 1, width - 2)
        depth = source.randint(2, height - 2)
        shift_x = source.randint(-100, 100)
        shift_y = source.randint(-100, 100)
        polygon = [
            (0, 0), (width, 0), (width, height),
            (second_x, height), (second_x, depth),
            (first_x, depth), (first_x, height), (0, height),
        ]
        cases.append([(x + shift_x, y + shift_y) for x, y in polygon])
    write_cases(arguments.out_dir, 0, cases)

    size = 200
    radius = 10_000_000
    maximum = [
        (
            round(radius * math.cos(2.0 * math.pi * index / size)),
            round(radius * math.sin(2.0 * math.pi * index / size)),
        )
        for index in range(size)
    ]
    write_cases(arguments.out_dir, 1, [maximum])


if __name__ == "__main__":
    main()
