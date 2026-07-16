#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path

Point = tuple[int, int]


def cross(a: Point, b: Point, c: Point) -> int:
    return (b[0] - a[0]) * (c[1] - a[1]) - (b[1] - a[1]) * (c[0] - a[0])


def convex_hull(points: list[Point]) -> list[Point]:
    ordered = sorted(set(points))
    if len(ordered) <= 1:
        return ordered
    lower: list[Point] = []
    for point in ordered:
        while len(lower) >= 2 and cross(lower[-2], lower[-1], point) <= 0:
            lower.pop()
        lower.append(point)
    upper: list[Point] = []
    for point in reversed(ordered):
        while len(upper) >= 2 and cross(upper[-2], upper[-1], point) <= 0:
            upper.pop()
        upper.append(point)
    return lower[:-1] + upper[:-1]


def contains(hull: list[Point], point: Point) -> int:
    if not hull:
        return 0
    if len(hull) == 1:
        return int(hull[0] == point)
    if len(hull) == 2:
        if cross(hull[0], hull[1], point) != 0:
            return 0
        return int(
            min(hull[0][0], hull[1][0]) <= point[0] <= max(hull[0][0], hull[1][0])
            and min(hull[0][1], hull[1][1]) <= point[1] <= max(hull[0][1], hull[1][1])
        )
    boundary = False
    for index, first in enumerate(hull):
        side = cross(first, hull[(index + 1) % len(hull)], point)
        if side < 0:
            return 0
        boundary |= side == 0
    return 1 if boundary else 2


def area2(hull: list[Point]) -> int:
    return abs(sum(
        hull[index][0] * hull[(index + 1) % len(hull)][1]
        - hull[index][1] * hull[(index + 1) % len(hull)][0]
        for index in range(len(hull))
    ))


def vertex_line(hull: list[Point]) -> str:
    values = [str(len(hull))]
    for x, y in hull:
        values.extend((str(x), str(y)))
    return " ".join(values)


def make_case(seed: int, additions: int, queries: int, bound: int) -> tuple[list[str], list[str]]:
    rng = random.Random(seed)
    points: list[Point] = []
    commands: list[str] = []
    output: list[str] = []
    operations = [0] * additions + [1] * queries
    rng.shuffle(operations)
    cached_hull: list[Point] | None = None
    for operation in operations:
        if operation == 0:
            if points and rng.randrange(8) == 0:
                point = rng.choice(points)
            elif rng.randrange(10) == 0:
                point = (rng.randrange(-bound, bound + 1), 0)
            else:
                point = (
                    rng.randrange(-bound, bound + 1),
                    rng.randrange(-bound, bound + 1),
                )
            points.append(point)
            cached_hull = None
            commands.append(f"ADD {point[0]} {point[1]}")
            continue

        if cached_hull is None:
            cached_hull = convex_hull(points)
        kind = rng.randrange(5)
        if kind == 0:
            point = (
                rng.randrange(-bound * 2, bound * 2 + 1),
                rng.randrange(-bound * 2, bound * 2 + 1),
            )
            commands.append(f"CONTAINS {point[0]} {point[1]}")
            output.append(str(contains(cached_hull, point)))
        elif kind == 1:
            commands.append("AREA")
            output.append(str(area2(cached_hull)))
        elif kind == 2:
            commands.append("SIZE")
            output.append(str(len(cached_hull)))
        elif kind == 3:
            direction = (
                rng.randrange(-bound, bound + 1),
                rng.randrange(-bound, bound + 1),
            )
            commands.append(f"SUPPORT {direction[0]} {direction[1]}")
            if not cached_hull:
                output.append("EMPTY")
            else:
                best = min(
                    cached_hull,
                    key=lambda p: (-(p[0] * direction[0] + p[1] * direction[1]), p),
                )
                output.append(f"{best[0]} {best[1]}")
        else:
            commands.append("VERTICES")
            output.append(vertex_line(cached_hull))
    return commands, output


def make_large_case() -> tuple[list[str], list[str]]:
    commands = ["CONTAINS 0 0", "AREA", "SIZE", "SUPPORT 1 1", "VERTICES"]
    output = ["0", "0", "0", "EMPTY", "0"]
    count = 100000
    for value in range(count):
        commands.append(f"ADD {value} {value * value}")
    hull = [(value, value * value) for value in range(count)]
    commands.extend(["SIZE", "AREA", "SUPPORT 0 -1", "SUPPORT 1 0"])
    output.extend([
        str(count), str(area2(hull)), "0 0",
        f"{count - 1} {(count - 1) * (count - 1)}",
    ])
    return commands, output


def write_case(directory: Path, index: int, case: tuple[list[str], list[str]]) -> None:
    commands, output = case
    (directory / f"case_{index:02d}.in").write_text(
        str(len(commands)) + "\n" + "\n".join(commands) + "\n", encoding="utf-8"
    )
    (directory / f"case_{index:02d}.out").write_text(
        "\n".join(output) + "\n", encoding="utf-8"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    directory = Path(args.out_dir)
    directory.mkdir(parents=True, exist_ok=True)
    write_case(directory, 0, make_case(2026071653, 120, 300, 10))
    write_case(directory, 1, make_case(2026071654, 1000, 1000, 10**9))
    write_case(directory, 2, make_case(2026071655, 2500, 800, 10**9))
    write_case(directory, 3, make_large_case())


if __name__ == "__main__":
    main()
