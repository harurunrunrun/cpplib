#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from convex_calipers_common import (  # noqa: E402
    convex_hull, encode_polygon, lens_polygon, random_hull, support_vertex,
    weak_polygon, write_case,
)


def query(polygon: list[tuple[int, int]], direction: tuple[int, int]) -> str:
    return f"{encode_polygon(polygon)} {direction[0]} {direction[1]}"


def answer(hull: list[tuple[int, int]], direction: tuple[int, int]) -> str:
    index, value = support_vertex(hull, direction)
    return f"{index} {value}"


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(2026071304)
    fixed_hulls = [
        [], [(4, -2)], [(-5, 1), (9, 1)],
        convex_hull([(0, 0), (6, 0), (6, 4), (0, 4)]),
    ]
    directions = [(3, -7), (0, 0), (1, 0), (0, 1)]
    write_case(
        args.out_dir, 0,
        [query(weak_polygon(hull, rng), direction)
         for hull, direction in zip(fixed_hulls, directions)],
        [answer(hull, direction)
         for hull, direction in zip(fixed_hulls, directions)],
    )

    items: list[tuple[list[tuple[int, int]], tuple[int, int]]] = []
    for _ in range(420):
        hull = random_hull(rng)
        direction = (rng.randrange(-100, 101), rng.randrange(-100, 101))
        items.append((hull, direction))
    write_case(
        args.out_dir, 1,
        [query(weak_polygon(hull, rng), direction) for hull, direction in items],
        [answer(hull, direction) for hull, direction in items],
    )

    large = lens_polygon(10000, 10**9)
    direction = (0, 1)
    write_case(
        args.out_dir, 2, [query(large, direction)],
        [answer(large, direction)],
    )


if __name__ == "__main__":
    main()
