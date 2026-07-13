#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from convex_calipers_common import (  # noqa: E402
    brute_farthest_distances, convex_hull, encode_polygon, lens_polygon,
    random_hull, squared_distance, weak_polygon, write_case,
)


def answer(hull: list[tuple[int, int]]) -> str:
    values = brute_farthest_distances(hull)
    return " ".join([str(len(values)), *map(str, values)])


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(2026071302)
    fixed_hulls = [
        [], [(2, -9)], [(-4, 1), (7, 1)],
        convex_hull([(0, 0), (5, 0), (5, 5), (0, 5)]),
        convex_hull([(-6, -3), (4, -7), (10, 0), (6, 9), (-5, 8)]),
    ]
    write_case(
        args.out_dir, 0,
        [encode_polygon(weak_polygon(hull, rng)) for hull in fixed_hulls],
        [answer(hull) for hull in fixed_hulls],
    )

    hulls = [random_hull(rng) for _ in range(300)]
    write_case(
        args.out_dir, 1,
        [encode_polygon(weak_polygon(hull, rng)) for hull in hulls],
        [answer(hull) for hull in hulls],
    )

    half_width = 10000
    height = 10**9
    large = lens_polygon(half_width, height)
    center_lower = (0, 0)
    center_upper = (0, height)
    distances = [
        squared_distance(point, center_upper) if index <= 2 * half_width
        else squared_distance(point, center_lower)
        for index, point in enumerate(large)
    ]
    write_case(
        args.out_dir, 2, [encode_polygon(large)],
        [" ".join([str(len(large)), *map(str, distances)])],
    )


if __name__ == "__main__":
    main()
