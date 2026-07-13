#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
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


def enumerate_boundary(polygon: list[Point], points: list[Point], unique: bool) -> list[int]:
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
    fast: bool,
) -> None:
    stem = f"case_{index:02d}"
    lines = [f"{len(polygon)} {len(points)} {int(unique)}"]
    lines += [f"{x} {y}" for x, y in polygon]
    lines += [f"{x} {y}" for x, y in points]
    answer = enumerate_boundary(polygon, points, unique)
    (out_dir / f"{stem}.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (out_dir / f"{stem}.out").write_text(
        " ".join(map(str, [int(fast), len(answer), *answer])) + "\n",
        encoding="utf-8",
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    square = [(0, 0), (6, 0), (6, 4), (0, 4)]
    points = [(0, 0), (2, 0), (6, 0), (6, 3), (4, 4), (0, 2), (2, 2), (9, 9)]
    weak = [(0, 0), (0, 0), (2, 0), (4, 0), (4, 2), (4, 4), (0, 4), (0, 2)]
    concave = [(0, 0), (6, 0), (3, 2), (6, 6), (0, 6)]
    cases = [
        ([], points, True, False),
        ([(0, 0)], points, False, False),
        ([(0, 0), (4, 0)], points, False, False),
        (square, points, True, True),
        (square, points, False, True),
        (list(reversed(square)), points, True, True),
        (list(reversed(square)), points, False, True),
        (weak, points, True, True),
        (weak, points, False, True),
        (list(reversed(weak)), points, False, True),
        (concave, points, True, False),
        (concave, points, False, False),
        (list(reversed(concave)), points, True, False),
        ([(0, 0), (2, 0), (4, 0)], points, False, False),
    ]
    for index, case in enumerate(cases):
        write_case(out_dir, index, *case)


if __name__ == "__main__":
    main()
