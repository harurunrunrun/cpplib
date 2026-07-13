#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from convex_calipers_common import (  # noqa: E402
    brute_tangents, convex_hull, encode_polygon, lens_polygon, linear_tangents,
    random_hull, weak_polygon, write_case,
)


def query(polygon: list[tuple[int, int]], point: tuple[int, int]) -> str:
    return f"{encode_polygon(polygon)} {point[0]} {point[1]}"


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(2026071305)
    fixed = [
        ([], (0, 0)),
        ([(2, 3)], (20, -4)),
        ([(-5, 0), (5, 0)], (0, 7)),
        ([(-5, 0), (5, 0)], (20, 0)),
        (convex_hull([(0, 0), (8, 0), (8, 6), (0, 6)]), (-7, 0)),
        (convex_hull([(0, 0), (8, 0), (8, 6), (0, 6)]), (20, 3)),
    ]
    write_case(
        args.out_dir, 0,
        [query(weak_polygon(hull, rng), point) for hull, point in fixed],
        [" ".join(map(str, brute_tangents(hull, point))) for hull, point in fixed],
    )

    items: list[tuple[list[tuple[int, int]], tuple[int, int]]] = []
    for _ in range(360):
        hull = random_hull(rng)
        maximum_x = max((point[0] for point in hull), default=0)
        point = (maximum_x + rng.randrange(100, 301), rng.randrange(-200, 201))
        items.append((hull, point))
    write_case(
        args.out_dir, 1,
        [query(weak_polygon(hull, rng), point) for hull, point in items],
        [" ".join(map(str, brute_tangents(hull, point)))
         for hull, point in items],
    )

    large = lens_polygon(10000, 10**9)
    point = (20000, 500000000)
    write_case(
        args.out_dir, 2, [query(large, point)],
        [" ".join(map(str, linear_tangents(large, point)))],
    )


if __name__ == "__main__":
    main()
