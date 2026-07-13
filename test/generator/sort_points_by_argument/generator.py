#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import functools
import random
from pathlib import Path

Point = tuple[int, int]


def sector(point: Point) -> int:
    x, y = point
    if y < 0:
        return -1
    if y == 0 and x >= 0:
        return 0
    return 1


def compare(left: Point, right: Point) -> int:
    left_sector = sector(left)
    right_sector = sector(right)
    if left_sector != right_sector:
        return -1 if left_sector < right_sector else 1
    cross = left[0] * right[1] - left[1] * right[0]
    if cross > 0:
        return -1
    if cross < 0:
        return 1
    if left[0] != right[0]:
        return -1 if left[0] < right[0] else 1
    if left[1] != right[1]:
        return -1 if left[1] < right[1] else 1
    return 0


def write_case(out_dir: Path, index: int, points: list[Point]) -> None:
    ordered = sorted(points, key=functools.cmp_to_key(compare))
    encode = lambda values: "".join(f"{x} {y}\n" for x, y in values)
    (out_dir / f"case_{index:02d}.in").write_text(
        f"{len(points)}\n" + encode(points), encoding="utf-8"
    )
    (out_dir / f"case_{index:02d}.out").write_text(
        encode(ordered), encoding="utf-8"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[list[Point]] = [
        [(0, 0)],
        [(-1, 0), (0, -1), (1, 0), (0, 1), (0, 0)],
        [(1, 1), (2, 2), (-1, -1), (-2, -2), (0, 0), (3, 3)],
        [(-1_000_000_000, -999_999_999), (-999_999_999, -999_999_998),
         (1_000_000_000, 999_999_999), (999_999_999, 999_999_998)],
    ]
    random_source = random.Random(2026071302)
    for size in [2, 3, 10, 31, 100, 500, 2000, 10000, 200000]:
        points = [
            (random_source.randrange(-10**9, 10**9 + 1),
             random_source.randrange(-10**9, 10**9 + 1))
            for _ in range(size)
        ]
        for _ in range(min(20, size)):
            scale = random_source.randrange(-1000, 1001)
            points[random_source.randrange(size)] = (1234567 * scale, 7654321 * scale)
        cases.append(points)

    for index, points in enumerate(cases):
        write_case(args.out_dir, index, points)


if __name__ == "__main__":
    main()
