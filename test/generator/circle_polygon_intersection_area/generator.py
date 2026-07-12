#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import math
from pathlib import Path


Point = tuple[float, float]


def dot(a: Point, b: Point) -> float:
    return a[0] * b[0] + a[1] * b[1]


def cross(a: Point, b: Point) -> float:
    return a[0] * b[1] - a[1] * b[0]


def edge_area(a: Point, b: Point, radius: float) -> float:
    direction = (b[0] - a[0], b[1] - a[1])
    aa = dot(direction, direction)
    if aa == 0:
        return 0.0
    bb = 2.0 * dot(a, direction)
    cc = dot(a, a) - radius * radius
    parameters = [0.0, 1.0]
    discriminant = bb * bb - 4.0 * aa * cc
    if discriminant >= 0.0:
        root = math.sqrt(discriminant)
        for value in ((-bb - root) / (2.0 * aa), (-bb + root) / (2.0 * aa)):
            if 0.0 < value < 1.0:
                parameters.append(value)
    parameters = sorted(set(parameters))
    result = 0.0
    for left, right in zip(parameters, parameters[1:]):
        p = (a[0] + direction[0] * left, a[1] + direction[1] * left)
        q = (a[0] + direction[0] * right, a[1] + direction[1] * right)
        middle = (
            a[0] + direction[0] * (left + right) / 2.0,
            a[1] + direction[1] * (left + right) / 2.0,
        )
        if dot(middle, middle) <= radius * radius + 1e-12:
            result += cross(p, q) / 2.0
        else:
            result += radius * radius * math.atan2(cross(p, q), dot(p, q)) / 2.0
    return result


def solve(center: Point, radius: float, polygon: list[Point]) -> float:
    shifted = [(x - center[0], y - center[1]) for x, y in polygon]
    return abs(sum(edge_area(shifted[i], shifted[(i + 1) % len(shifted)], radius) for i in range(len(shifted))))


def write_case(out_dir: Path, index: int, center: Point, radius: float, polygon: list[Point]) -> None:
    stem = f"case_{index:02d}"
    lines = [f"{len(polygon)} {center[0]} {center[1]} {radius}"]
    lines += [f"{x} {y}" for x, y in polygon]
    (out_dir / f"{stem}.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (out_dir / f"{stem}.out").write_text(f"{solve(center, radius, polygon):.8f}\n", encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    cases = [
        ((0.0, 0.0), 1.0, [(-3.0, -3.0), (3.0, -3.0), (3.0, 3.0), (-3.0, 3.0)]),
        ((0.0, 0.0), 10.0, [(-2.0, -1.0), (3.0, -1.0), (3.0, 2.0), (-2.0, 2.0)]),
        ((0.0, 0.0), 2.0, [(0.0, -5.0), (5.0, -5.0), (5.0, 5.0), (0.0, 5.0)]),
        ((1.0, -2.0), 3.0, [(1.0, -2.0), (7.0, -2.0), (1.0, 4.0)]),
        ((0.0, 0.0), 2.0, [(5.0, 5.0), (7.0, 5.0), (7.0, 7.0), (5.0, 7.0)]),
        ((0.5, 0.25), 2.5, [(-4.0, -1.0), (-1.0, -3.0), (3.0, -2.0), (4.0, 2.0), (0.0, 4.0), (-3.0, 2.0)]),
    ]
    for index, (center, radius, polygon) in enumerate(cases):
        write_case(out_dir, index, center, radius, polygon)
        write_case(out_dir, index + len(cases), center, radius, list(reversed(polygon)))


if __name__ == "__main__":
    main()
