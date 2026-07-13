#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


Point = tuple[int, int]


def sub(a: Point, b: Point) -> Point:
    return (a[0] - b[0], a[1] - b[1])


def dot(a: Point, b: Point) -> int:
    return a[0] * b[0] + a[1] * b[1]


def cross(a: Point, b: Point) -> int:
    return a[0] * b[1] - a[1] * b[0]


def on_segment(a: Point, b: Point, point: Point) -> bool:
    return cross(sub(b, a), sub(point, a)) == 0 and dot(sub(point, a), sub(point, b)) <= 0


def solve(polygon: list[Point], points: list[Point], unique: bool) -> list[int]:
    used = [False] * len(points)
    result: list[int] = []
    for edge, first in enumerate(polygon):
        second = polygon[(edge + 1) % len(polygon)]
        base = sub(second, first)
        current: list[tuple[int, int]] = []
        for index, point in enumerate(points):
            if unique and used[index]:
                continue
            if on_segment(first, second, point):
                current.append((dot(sub(point, first), base), index))
        current.sort()
        for _, index in current:
            result.append(index)
            if unique:
                used[index] = True
    return result


def write_case(
    out_dir: Path,
    index: int,
    polygon: list[Point],
    points: list[Point],
    unique: bool,
) -> None:
    stem = f"case_{index:02d}"
    lines = [f"{len(polygon)} {len(points)} {int(unique)}"]
    lines += [f"{x} {y}" for x, y in polygon]
    lines += [f"{x} {y}" for x, y in points]
    answer = solve(polygon, points, unique)
    (out_dir / f"{stem}.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (out_dir / f"{stem}.out").write_text(
        " ".join(map(str, [len(answer), *answer])) + "\n",
        encoding="utf-8",
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[tuple[list[Point], list[Point], bool]] = []
    square = [(0, 0), (6, 0), (6, 4), (0, 4)]
    square_points = [(0, 0), (3, 0), (6, 0), (6, 2), (6, 4), (2, 4), (0, 4), (0, 1), (2, 2)]
    cases += [
        ([], [(0, 0), (1, 2)], True),
        (square, square_points, True),
        (square, square_points, False),
        (list(reversed(square)), square_points, True),
        (list(reversed(square)), square_points, False),
        (
            [(0, 0), (0, 0), (3, 0), (3, 3), (0, 3)],
            [(0, 0), (1, 0), (3, 1), (0, 2), (1, 1)],
            True,
        ),
        (
            [(0, 0), (0, 0), (3, 0), (3, 3), (0, 3)],
            [(0, 0), (1, 0), (3, 1), (0, 2), (1, 1)],
            False,
        ),
        (
            [(0, 0), (6, 0), (3, 2), (6, 6), (0, 6)],
            [(0, 0), (4, 0), (3, 2), (4, 4), (1, 6), (3, 3)],
            True,
        ),
        ([(0, 0), (4, 0)], [(0, 0), (1, 0), (4, 0), (2, 1)], False),
    ]

    rng = random.Random(0xB0A1D)
    for _ in range(12):
        left = rng.randint(-20, 0)
        right = rng.randint(1, 20)
        bottom = rng.randint(-20, 0)
        top = rng.randint(1, 20)
        polygon = [(left, bottom), (right, bottom), (right, top), (left, top)]
        if rng.randrange(2):
            polygon.reverse()
        points = [
            (left, bottom),
            (right, top),
            (rng.randint(left, right), bottom),
            (right, rng.randint(bottom, top)),
            (rng.randint(left, right), top),
            (left, rng.randint(bottom, top)),
        ]
        points += [(rng.randint(-25, 25), rng.randint(-25, 25)) for _ in range(20)]
        cases.append((polygon, points, bool(rng.randrange(2))))

    for index, (polygon, points, unique) in enumerate(cases):
        write_case(out_dir, index, polygon, points, unique)


if __name__ == "__main__":
    main()
