#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import math
import random
from pathlib import Path


def union_area(circles: list[tuple[float, float, float]]) -> float:
    result = 0.0
    two_pi = 2.0 * math.pi
    for index, (x, y, radius) in enumerate(circles):
        if radius == 0.0:
            continue
        intervals: list[tuple[float, float]] = []
        covered = False
        for other_index, (ox, oy, other_radius) in enumerate(circles):
            if index == other_index or other_radius == 0.0:
                continue
            dx, dy = ox - x, oy - y
            distance = math.hypot(dx, dy)
            if distance == 0.0 and radius == other_radius:
                if other_index < index:
                    covered = True
                    break
                continue
            if distance + radius <= other_radius:
                covered = True
                break
            if distance >= radius + other_radius or distance + other_radius <= radius:
                continue
            cosine = max(-1.0, min(1.0, (radius * radius + distance * distance
                                        - other_radius * other_radius)
                                       / (2.0 * radius * distance)))
            width = math.acos(cosine)
            center = math.atan2(dy, dx)
            left, right = center - width, center + width
            while left < 0.0:
                left += two_pi
                right += two_pi
            while left >= two_pi:
                left -= two_pi
                right -= two_pi
            if right <= two_pi:
                intervals.append((left, right))
            else:
                intervals.extend(((left, two_pi), (0.0, right - two_pi)))
        if covered:
            continue
        intervals.sort()

        def arc(begin: float, end: float) -> float:
            return 0.5 * (radius * x * (math.sin(end) - math.sin(begin))
                          + radius * y * (math.cos(begin) - math.cos(end))
                          + radius * radius * (end - begin))

        until = 0.0
        for left, right in intervals:
            if left > until:
                result += arc(until, left)
            until = max(until, right)
        if until < two_pi:
            result += arc(until, two_pi)
    return result


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    directory = Path(args.out_dir)
    directory.mkdir(parents=True, exist_ok=True)

    rng = random.Random(2026071622)
    cases: list[list[tuple[float, float, float]]] = [
        [], [(0.0, 0.0, 1.0)], [(0.0, 0.0, 2.0), (0.0, 0.0, 1.0)],
        [(0.0, 0.0, 1.0), (1.0, 0.0, 1.0)],
        [(-10.0, 0.0, 2.0), (10.0, 0.0, 3.0)],
    ]
    for _ in range(40):
        count = rng.randrange(1, 30)
        cases.append([
            (float(rng.randrange(-100, 101)), float(rng.randrange(-100, 101)),
             float(rng.randrange(0, 31)))
            for _ in range(count)
        ])
    lines = [str(len(cases))]
    for circles in cases:
        lines.append(str(len(circles)))
        lines.extend(f"{x:.17g} {y:.17g} {radius:.17g}" for x, y, radius in circles)
        lines.append(f"{union_area(circles):.17g} 1e-9")
    (directory / "case_00.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (directory / "case_00.out").write_text("1\n" * len(cases), encoding="utf-8")


if __name__ == "__main__":
    main()

