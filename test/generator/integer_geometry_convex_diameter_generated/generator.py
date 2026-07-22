#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import math
import random
from pathlib import Path

MINIMUM = -(1 << 63)
MAXIMUM = (1 << 63) - 1
MAXIMUM_U128 = (1 << 128) - 1
Point = tuple[int, int]


def cross(first: Point, second: Point, third: Point) -> int:
    return (
        (second[0] - first[0]) * (third[1] - first[1])
        - (second[1] - first[1]) * (third[0] - first[0])
    )


def convex_hull(points: list[Point]) -> list[Point]:
    points = sorted(set(points))
    if len(points) <= 1:
        return points

    lower: list[Point] = []
    for point in points:
        while (
            len(lower) >= 2
            and cross(lower[-2], lower[-1], point) <= 0
        ):
            lower.pop()
        lower.append(point)

    upper: list[Point] = []
    for point in reversed(points):
        while (
            len(upper) >= 2
            and cross(upper[-2], upper[-1], point) <= 0
        ):
            upper.pop()
        upper.append(point)
    return lower[:-1] + upper[:-1]


def add_collinear_boundary_points(polygon: list[Point]) -> list[Point]:
    if len(polygon) < 3:
        return polygon
    result: list[Point] = []
    for index, first in enumerate(polygon):
        second = polygon[(index + 1) % len(polygon)]
        result.append(first)
        delta_x = second[0] - first[0]
        delta_y = second[1] - first[1]
        divisor = math.gcd(abs(delta_x), abs(delta_y))
        for step in range(1, min(divisor, 4)):
            result.append((
                first[0] + delta_x * step // divisor,
                first[1] + delta_y * step // divisor,
            ))
    return result


def squared_distance(first: Point, second: Point) -> int:
    return (
        (first[0] - second[0]) ** 2
        + (first[1] - second[1]) ** 2
    )


def answer(polygon: list[Point]) -> str:
    if not polygon:
        return "none"
    diameter = max(
        squared_distance(first, second)
        for first in polygon
        for second in polygon
    )
    return str(diameter) if diameter <= MAXIMUM_U128 else "overflow"


def cyclic_variants(polygon: list[Point]) -> list[list[Point]]:
    if not polygon:
        return [[]]
    result: list[list[Point]] = []
    for oriented in (polygon, list(reversed(polygon))):
        for shift in range(len(oriented)):
            result.append(oriented[shift:] + oriented[:shift])
    return result


def write_cases(
    directory: Path,
    name: str,
    polygons: list[list[Point]],
) -> None:
    input_lines = [str(len(polygons))]
    output_lines: list[str] = []
    for polygon in polygons:
        input_lines.append(str(len(polygon)))
        input_lines.extend(f"{x} {y}" for x, y in polygon)
        output_lines.append(answer(polygon))
    (directory / f"{name}.in").write_text(
        "\n".join(input_lines) + "\n", encoding="utf-8"
    )
    (directory / f"{name}.out").write_text(
        "\n".join(output_lines) + "\n", encoding="utf-8"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)

    regression = [(29, -27), (20, -28), (11, -29), (-12, -3)]
    collinear = [(0, 0), (1, 0), (2, 0), (3, 0)]
    extreme_collinear = [
        (MINIMUM, 0),
        (-1, 0),
        (0, 0),
        (MAXIMUM, 0),
    ]
    extreme_thin = [
        (MINIMUM, 0),
        (MAXIMUM, 0),
        (MAXIMUM, 1),
        (MINIMUM, 1),
    ]
    extreme_square = [
        (MINIMUM, MINIMUM),
        (MAXIMUM, MINIMUM),
        (MAXIMUM, MAXIMUM),
        (MINIMUM, MAXIMUM),
    ]

    fixed = [[], [(7, -3)], [(-4, 8), (9, -2)]]
    for polygon in (
        regression,
        collinear,
        extreme_collinear,
        extreme_thin,
        extreme_square,
    ):
        fixed.extend(cyclic_variants(polygon))
    write_cases(args.out_dir, "case_00_fixed_boundary_overflow", fixed)

    generator = random.Random(2026072204)
    counterexamples: list[list[Point]] = []
    for iteration in range(2048):
        points = [
            (
                generator.randrange(-1_000_000_000, 1_000_000_001),
                generator.randrange(-1_000_000_000, 1_000_000_001),
            )
            for _ in range(generator.randrange(3, 31))
        ]
        polygon = add_collinear_boundary_points(convex_hull(points))
        if polygon:
            shift = generator.randrange(len(polygon))
            polygon = polygon[shift:] + polygon[:shift]
        if iteration % 2:
            polygon.reverse()
        counterexamples.append(polygon)
    write_cases(
        args.out_dir,
        "case_01_random_collinear_orientation_rotation",
        counterexamples,
    )


if __name__ == "__main__":
    main()
