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


def pair_circle(first: Point, second: Point) -> Circle:
    x = Fraction(first[0] + second[0], 2)
    y = Fraction(first[1] + second[1], 2)
    return x, y, squared_distance(x, y, first)


def triple_circle(first: Point, second: Point, third: Point) -> Circle | None:
    x1, y1 = first
    x2, y2 = second
    x3, y3 = third
    determinant = 2 * (x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2))
    if determinant == 0:
        return None
    first_norm = x1 * x1 + y1 * y1
    second_norm = x2 * x2 + y2 * y2
    third_norm = x3 * x3 + y3 * y3
    x = Fraction(
        first_norm * (y2 - y3)
        + second_norm * (y3 - y1)
        + third_norm * (y1 - y2),
        determinant,
    )
    y = Fraction(
        first_norm * (x3 - x2)
        + second_norm * (x1 - x3)
        + third_norm * (x2 - x1),
        determinant,
    )
    return x, y, squared_distance(x, y, first)


def boundary_flags(points: list[Point]) -> str:
    candidates: list[Circle] = [
        (Fraction(x), Fraction(y), Fraction(0)) for x, y in points
    ]
    candidates.extend(pair_circle(*pair) for pair in itertools.combinations(points, 2))
    for triple in itertools.combinations(points, 3):
        circle = triple_circle(*triple)
        if circle is not None:
            candidates.append(circle)
    containing = [
        circle
        for circle in candidates
        if all(squared_distance(circle[0], circle[1], point) <= circle[2] for point in points)
    ]
    best = min(containing, key=lambda circle: circle[2])
    return "".join(
        "1" if squared_distance(best[0], best[1], point) == best[2] else "0"
        for point in points
    )


def write_case(out_dir: Path, index: int, points: list[Point]) -> None:
    lines = [str(len(points))]
    lines.extend(f"{x} {y}" for x, y in points)
    (out_dir / f"case_{index:02d}.in").write_text(
        "\n".join(lines) + "\n", encoding="utf-8"
    )
    (out_dir / f"case_{index:02d}.out").write_text(
        boundary_flags(points) + "\n", encoding="utf-8"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    arguments = parser.parse_args()
    out_dir = Path(arguments.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    fixed: list[list[Point]] = [
        [(0, 0)],
        [(-1, 0), (1, 0), (0, 0)],
        [(0, 0), (2, 0), (0, 2), (1, 1)],
        [(0, 0), (0, 0), (0, 0)],
        [(-3, 0), (-1, 0), (2, 0), (5, 0)],
    ]
    for index, points in enumerate(fixed):
        write_case(out_dir, index, points)

    rng = random.Random(20260714)
    for index in range(len(fixed), len(fixed) + 18):
        size = rng.randrange(1, 11)
        points = [(rng.randrange(-8, 9), rng.randrange(-8, 9)) for _ in range(size)]
        write_case(out_dir, index, points)


if __name__ == "__main__":
    main()
