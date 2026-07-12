#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
from pathlib import Path


Point = tuple[int, int]


def sub(a: Point, b: Point) -> Point:
    return (a[0] - b[0], a[1] - b[1])


def dot(a: Point, b: Point) -> int:
    return a[0] * b[0] + a[1] * b[1]


def cross(a: Point, b: Point) -> int:
    return a[0] * b[1] - a[1] * b[0]


def on_segment(a: Point, b: Point, p: Point) -> bool:
    return cross(sub(b, a), sub(p, a)) == 0 and dot(sub(p, a), sub(p, b)) <= 0


def solve(poly: list[Point], points: list[Point]) -> list[int]:
    used = [False] * len(points)
    result: list[int] = []
    n = len(poly)
    for i in range(n):
        a = poly[i]
        b = poly[(i + 1) % n]
        base = sub(b, a)
        base_norm = dot(base, base)
        current: list[tuple[int, int]] = []
        for j, p in enumerate(points):
            if used[j] or not on_segment(a, b, p):
                continue
            parameter = 0 if base_norm == 0 else dot(sub(p, a), base)
            current.append((parameter, j))
        current.sort()
        for _, j in current:
            result.append(j)
            used[j] = True
    return result


def write_case(out_dir: Path, idx: int, poly: list[Point], points: list[Point]) -> None:
    name = f"case_{idx:02d}"
    lines = [f"{len(poly)} {len(points)}"]
    lines += [f"{x} {y}" for x, y in poly]
    lines += [f"{x} {y}" for x, y in points]
    (out_dir / f"{name}.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    result = solve(poly, points)
    (out_dir / f"{name}.out").write_text(" ".join([str(len(result)), *map(str, result)]) + "\n", encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()

    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    write_case(
        out_dir,
        0,
        [(0, 0), (4, 0), (4, 4), (0, 4)],
        [(0, 0), (2, 0), (4, 2), (1, 1), (0, 3), (2, 4)],
    )
    write_case(
        out_dir,
        1,
        [(0, 0), (4, 0), (2, 2), (4, 4), (0, 4)],
        [(2, 0), (3, 1), (2, 2), (3, 3), (1, 4), (2, 3), (5, 5)],
    )
    write_case(
        out_dir,
        2,
        [(-3, -1), (2, -1), (2, 3), (-3, 3)],
        [(-3, -1), (-1, -1), (2, 1), (0, 3), (-3, 2), (0, 0), (4, 4)],
    )
    write_case(
        out_dir,
        3,
        [(0, 0), (0, 0), (2, 0), (2, 2), (0, 2)],
        [(0, 0), (1, 0), (1, 1), (2, 1), (0, 2), (9, 9)],
    )


if __name__ == "__main__":
    main()
