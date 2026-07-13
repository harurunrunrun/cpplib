#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import itertools
import random
from fractions import Fraction
from pathlib import Path

Point = tuple[int, int]
Circle = tuple[Fraction, Fraction, Fraction]


def squared_distance(x: Fraction, y: Fraction, point: Point) -> Fraction:
    return (x - point[0]) ** 2 + (y - point[1]) ** 2


def diameter_circle(first: Point, second: Point) -> Circle:
    x = Fraction(first[0] + second[0], 2)
    y = Fraction(first[1] + second[1], 2)
    return x, y, squared_distance(x, y, first)


def circumcircle(first: Point, second: Point, third: Point) -> Circle | None:
    ax, ay = first
    bx, by = second
    cx, cy = third
    denominator = 2 * (ax * (by - cy) + bx * (cy - ay) + cx * (ay - by))
    if denominator == 0:
        return None
    a_norm = ax * ax + ay * ay
    b_norm = bx * bx + by * by
    c_norm = cx * cx + cy * cy
    x = Fraction(
        a_norm * (by - cy) + b_norm * (cy - ay) + c_norm * (ay - by),
        denominator,
    )
    y = Fraction(
        a_norm * (cx - bx) + b_norm * (ax - cx) + c_norm * (bx - ax),
        denominator,
    )
    return x, y, squared_distance(x, y, first)


def boundary(points: list[Point]) -> str:
    if not points:
        return ""
    candidates: list[Circle] = [
        (Fraction(x), Fraction(y), Fraction(0)) for x, y in points
    ]
    candidates.extend(diameter_circle(a, b) for a, b in itertools.combinations(points, 2))
    for triple in itertools.combinations(points, 3):
        circle = circumcircle(*triple)
        if circle is not None:
            candidates.append(circle)

    best: Circle | None = None
    for circle in candidates:
        x, y, radius = circle
        if all(squared_distance(x, y, point) <= radius for point in points):
            if best is None or radius < best[2]:
                best = circle
    assert best is not None
    x, y, radius = best
    return "".join(
        "1" if squared_distance(x, y, point) == radius else "0"
        for point in points
    )


def write_case(out_dir: Path, index: int, points: list[Point], answer: str | None = None) -> None:
    input_data = f"{len(points)}\n" + "".join(f"{x} {y}\n" for x, y in points)
    output_data = (boundary(points) if answer is None else answer) + "\n"
    (out_dir / f"case_{index:02d}.in").write_text(input_data, encoding="utf-8")
    (out_dir / f"case_{index:02d}.out").write_text(output_data, encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[list[Point]] = [
        [],
        [(3, -7)],
        [(-5, 0), (5, 0)],
        [(-5, 0), (5, 0), (0, 0), (1, 0), (-2, 0)],
        [(0, 0), (4, 0), (0, 3), (1, 1)],
        [(3, 4), (-3, 4), (3, -4), (-3, -4), (0, 0)],
        [(0, 0), (0, 0), (1, 0), (-1, 0)],
    ]
    random_source = random.Random(2026071303)
    for _ in range(30):
        size = random_source.randrange(2, 14)
        points = list({
            (random_source.randrange(-20, 21), random_source.randrange(-20, 21))
            for _ in range(size * 2)
        })[:size]
        cases.append(points)

    for index, points in enumerate(cases):
        write_case(args.out_dir, index, points)

    collinear = [(coordinate, 0) for coordinate in range(-1000, 1001)]
    write_case(
        args.out_dir,
        len(cases),
        collinear,
        "1" + "0" * (len(collinear) - 2) + "1",
    )

    maximum = [(-10000, 0), (10000, 0)] + [(0, 0)] * 99998
    write_case(
        args.out_dir,
        len(cases) + 1,
        maximum,
        "11" + "0" * (len(maximum) - 2),
    )


if __name__ == "__main__":
    main()
