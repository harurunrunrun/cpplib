#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import math
from pathlib import Path


Point = tuple[float, float]


def dot(first: Point, second: Point) -> float:
    return first[0] * second[0] + first[1] * second[1]


def cross(first: Point, second: Point) -> float:
    return first[0] * second[1] - first[1] * second[0]


def edge_area(first: Point, second: Point, radius: float) -> float:
    direction = (second[0] - first[0], second[1] - first[1])
    aa = dot(direction, direction)
    if aa == 0.0:
        return 0.0
    bb = 2.0 * dot(first, direction)
    cc = dot(first, first) - radius * radius
    parameters = [0.0, 1.0]
    discriminant = bb * bb - 4.0 * aa * cc
    if discriminant >= -1e-12:
        root = math.sqrt(max(0.0, discriminant))
        for parameter in ((-bb - root) / (2.0 * aa), (-bb + root) / (2.0 * aa)):
            if 1e-12 < parameter < 1.0 - 1e-12:
                parameters.append(parameter)
    parameters.sort()
    unique_parameters: list[float] = []
    for parameter in parameters:
        if not unique_parameters or abs(parameter - unique_parameters[-1]) > 1e-12:
            unique_parameters.append(parameter)

    result = 0.0
    for left, right in zip(unique_parameters, unique_parameters[1:]):
        p = (first[0] + direction[0] * left, first[1] + direction[1] * left)
        q = (first[0] + direction[0] * right, first[1] + direction[1] * right)
        middle = (
            first[0] + direction[0] * (left + right) / 2.0,
            first[1] + direction[1] * (left + right) / 2.0,
        )
        if dot(middle, middle) <= radius * radius + 1e-12:
            result += cross(p, q) / 2.0
        else:
            result += radius * radius * math.atan2(cross(p, q), dot(p, q)) / 2.0
    return result


def solve(center: Point, radius: float, polygon: list[Point]) -> float:
    if radius == 0.0 or len(polygon) < 2:
        return 0.0
    shifted = [(x - center[0], y - center[1]) for x, y in polygon]
    result = sum(
        edge_area(shifted[index], shifted[(index + 1) % len(shifted)], radius)
        for index in range(len(shifted))
    )
    return 0.0 if abs(result) < 0.5e-9 else result


def write_case(
    out_dir: Path,
    index: int,
    center: Point,
    radius: float,
    polygon: list[Point],
) -> None:
    stem = f"case_{index:02d}"
    lines = [f"{len(polygon)} {center[0]} {center[1]} {radius}"]
    lines += [f"{x} {y}" for x, y in polygon]
    (out_dir / f"{stem}.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (out_dir / f"{stem}.out").write_text(
        f"{solve(center, radius, polygon):.8f}\n",
        encoding="utf-8",
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    cases = [
        ((0.0, 0.0), 1.0, [(-3.0, -3.0), (3.0, -3.0), (3.0, 3.0), (-3.0, 3.0)]),
        ((0.0, 0.0), 1.0, [(-3.0, 3.0), (3.0, 3.0), (3.0, -3.0), (-3.0, -3.0)]),
        ((0.0, 0.0), 10.0, [(0.0, 0.0), (2.0, 0.0), (0.0, 3.0)]),
        ((0.0, 0.0), 10.0, [(0.0, 3.0), (2.0, 0.0), (0.0, 0.0)]),
        ((0.0, 0.0), 2.0, [(0.0, -5.0), (5.0, -5.0), (5.0, 5.0), (0.0, 5.0)]),
        ((0.0, 0.0), 2.0, [(0.0, 5.0), (5.0, 5.0), (5.0, -5.0), (0.0, -5.0)]),
        ((1.5, -2.0), 2.5, [(-3.0, -1.0), (-1.0, -4.0), (4.0, -3.0), (5.0, 2.0), (0.0, 4.0)]),
        ((1.5, -2.0), 2.5, [(0.0, 4.0), (5.0, 2.0), (4.0, -3.0), (-1.0, -4.0), (-3.0, -1.0)]),
        ((0.0, 0.0), 0.0, [(-1.0, -1.0), (1.0, -1.0), (1.0, 1.0), (-1.0, 1.0)]),
        ((0.0, 0.0), 3.0, []),
        ((0.0, 0.0), 3.0, [(1.0, 1.0)]),
    ]
    for index, case in enumerate(cases):
        write_case(out_dir, index, *case)


if __name__ == "__main__":
    main()
