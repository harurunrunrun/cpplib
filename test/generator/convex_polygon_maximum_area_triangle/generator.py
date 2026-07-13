#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from convex_calipers_common import (  # noqa: E402
    brute_maximum_triangle, calipers_maximum_triangle, convex_hull,
    encode_polygon, lens_polygon, random_hull, weak_polygon, write_case,
)


def answer(hull: list[tuple[int, int]]) -> str:
    valid, doubled_area = brute_maximum_triangle(hull)
    return f"{int(valid)} {doubled_area}"


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(2026071303)
    fixed_hulls = [
        [], [(0, 0)], [(-3, 2), (8, 2)],
        convex_hull([(0, 0), (7, 0), (0, 5)]),
        convex_hull([(0, 0), (6, 0), (8, 3), (4, 9), (-2, 5)]),
    ]
    write_case(
        args.out_dir, 0,
        [encode_polygon(weak_polygon(hull, rng)) for hull in fixed_hulls],
        [answer(hull) for hull in fixed_hulls],
    )

    hulls = [random_hull(rng, 22) for _ in range(260)]
    write_case(
        args.out_dir, 1,
        [encode_polygon(weak_polygon(hull, rng)) for hull in hulls],
        [answer(hull) for hull in hulls],
    )

    large = lens_polygon(600, 10**6)
    write_case(
        args.out_dir, 2, [encode_polygon(large)],
        [f"1 {calipers_maximum_triangle(large)}"],
    )


if __name__ == "__main__":
    main()
