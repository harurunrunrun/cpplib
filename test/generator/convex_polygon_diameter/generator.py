#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from convex_calipers_common import (  # noqa: E402
    brute_diameter, convex_hull, encode_polygon, lens_polygon, random_hull,
    weak_polygon, write_case,
)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(2026071301)
    fixed_hulls = [
        [], [(3, 4)], [(-5, 0), (8, 0)],
        convex_hull([(0, 0), (4, 0), (4, 3), (0, 3)]),
        convex_hull([(-7, -2), (5, -5), (9, 4), (0, 11), (-8, 5)]),
    ]
    fixed = [weak_polygon(hull, rng) for hull in fixed_hulls]
    write_case(
        args.out_dir, 0,
        [encode_polygon(polygon) for polygon in fixed],
        [str(brute_diameter(hull)) for hull in fixed_hulls],
    )

    hulls = [random_hull(rng) for _ in range(320)]
    write_case(
        args.out_dir, 1,
        [encode_polygon(weak_polygon(hull, rng)) for hull in hulls],
        [str(brute_diameter(hull)) for hull in hulls],
    )

    large = lens_polygon(10000, 10**9)
    write_case(args.out_dir, 2, [encode_polygon(large)], [str(10**18)])


if __name__ == "__main__":
    main()
