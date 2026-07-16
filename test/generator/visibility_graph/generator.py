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


def write_complete_convex_case(
    output_directory: Path,
    index: int,
    polygon: list[Point],
) -> None:
    """Write the analytically known complete graph of a strict convex polygon."""

    size = len(polygon)
    turns = [
        cross(polygon[i], polygon[(i + 1) % size], polygon[(i + 2) % size])
        for i in range(size)
    ]
    assert all(turn > 0 for turn in turns) or all(turn < 0 for turn in turns)
    name = f"case_{index:02d}"
    with (output_directory / f"{name}.in").open(
        "w", encoding="utf-8", newline="\n"
    ) as input_file, (output_directory / f"{name}.out").open(
        "w", encoding="utf-8", newline="\n"
    ) as output_file:
        input_file.write(f"1\n{size}\n")
        input_file.writelines(f"{x} {y}\n" for x, y in polygon)
        output_file.write(f"{size * (size - 1) // 2}\n")
        output_file.writelines(
            f"{first} {second}\n"
            for first in range(size)
            for second in range(first + 1, size)
        )


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
        [(0, 0), (5, 0), (10, 0), (10, 5),
         (10, 10), (5, 10), (0, 10), (0, 5)],
        [(0, 0), (4, 0), (8, 0), (8, 8), (6, 8),
         (6, 3), (4, 3), (2, 3), (2, 8), (0, 8)],
        [(0, 0), (12, 0), (12, 12), (9, 12), (9, 4),
         (7, 4), (7, 10), (5, 10), (5, 4), (3, 4),
         (3, 12), (0, 12)],
    ]
    source = random.Random(2026071508)
    for case_index in range(80):
        size = source.randint(6, 30)
        offset = source.random() * 2.0 * math.pi
        radial = []
        for vertex in range(size):
            angle = offset + 2.0 * math.pi * vertex / size
            radius = source.randint(20_000, 100_000)
            radial.append((
                round(radius * math.cos(angle)),
                round(radius * math.sin(angle)),
            ))
        if case_index % 2 != 0:
            radial.reverse()
        cases.append(radial)
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

    performance_size = 800
    performance_radius = 1_000_000_000
    performance = [
        (
            round(performance_radius * math.cos(
                2.0 * math.pi * index / performance_size
            )),
            round(performance_radius * math.sin(
                2.0 * math.pi * index / performance_size
            )),
        )
        for index in range(performance_size)
    ]
    write_complete_convex_case(arguments.out_dir, 2, performance)


if __name__ == "__main__":
    main()
