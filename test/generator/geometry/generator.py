#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


Point = tuple[int, int]


def sign(x: int) -> int:
    return (x > 0) - (x < 0)


def sub(a: Point, b: Point) -> Point:
    return (a[0] - b[0], a[1] - b[1])


def cross(a: Point, b: Point) -> int:
    return a[0] * b[1] - a[1] * b[0]


def dot(a: Point, b: Point) -> int:
    return a[0] * b[0] + a[1] * b[1]


def norm(a: Point) -> int:
    return dot(a, a)


def ccw(a: Point, b: Point, c: Point) -> int:
    b = sub(b, a)
    c = sub(c, a)
    cr = sign(cross(b, c))
    if cr > 0:
        return 1
    if cr < 0:
        return -1
    if sign(dot(b, c)) < 0:
        return 2
    if sign(norm(b) - norm(c)) < 0:
        return -2
    return 0


def intersect(s: tuple[Point, Point], t: tuple[Point, Point]) -> int:
    return int(ccw(s[0], s[1], t[0]) * ccw(s[0], s[1], t[1]) <= 0 and ccw(t[0], t[1], s[0]) * ccw(t[0], t[1], s[1]) <= 0)


def contains(poly: list[Point], p: Point) -> int:
    inside = False
    n = len(poly)
    for i in range(n):
        a = sub(poly[i], p)
        b = sub(poly[(i + 1) % n], p)
        if cross(a, b) == 0 and dot(a, b) <= 0:
            return 1
        if a[1] > b[1]:
            a, b = b, a
        if a[1] <= 0 < b[1] and cross(a, b) > 0:
            inside = not inside
    return 2 if inside else 0


def solve(queries: list[str]) -> str:
    out: list[str] = []
    for query in queries:
        xs = query.split()
        if xs[0] == "CCW":
            a = (int(xs[1]), int(xs[2]))
            b = (int(xs[3]), int(xs[4]))
            c = (int(xs[5]), int(xs[6]))
            out.append(str(ccw(a, b, c)))
        elif xs[0] == "INTERSECT":
            nums = list(map(int, xs[1:]))
            out.append(str(intersect(((nums[0], nums[1]), (nums[2], nums[3])), ((nums[4], nums[5]), (nums[6], nums[7])))))
        elif xs[0] == "CONTAINS":
            n = int(xs[1])
            nums = list(map(int, xs[2:]))
            poly = [(nums[2 * i], nums[2 * i + 1]) for i in range(n)]
            p = (nums[2 * n], nums[2 * n + 1])
            out.append(str(contains(poly, p)))
    return "\n".join(out) + "\n"


def write_case(out_dir: Path, idx: int, queries: list[str]) -> None:
    name = f"case_{idx:02d}"
    data = "\n".join([str(len(queries)), *queries]) + "\n"
    (out_dir / f"{name}.in").write_text(data, encoding="utf-8")
    (out_dir / f"{name}.out").write_text(solve(queries), encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    fixed = [
        "CCW 0 0 2 0 1 1",
        "CCW 0 0 2 0 1 -1",
        "CCW 0 0 2 0 3 0",
        "INTERSECT 0 0 2 2 0 2 2 0",
        "INTERSECT 0 0 1 0 2 0 3 0",
        "CONTAINS 4 0 0 4 0 4 4 0 4 2 2",
        "CONTAINS 4 0 0 4 0 4 4 0 4 4 2",
        "CONTAINS 4 0 0 4 0 4 4 0 4 5 2",
    ]
    write_case(out_dir, 0, fixed)

    rng = random.Random(20260907)
    queries: list[str] = []
    for _ in range(80):
        kind = rng.randrange(3)
        if kind == 0:
            nums = [rng.randrange(-5, 6) for _ in range(6)]
            queries.append("CCW " + " ".join(map(str, nums)))
        elif kind == 1:
            nums = [rng.randrange(-5, 6) for _ in range(8)]
            queries.append("INTERSECT " + " ".join(map(str, nums)))
        else:
            x1, y1 = rng.randrange(-5, 1), rng.randrange(-5, 1)
            x2, y2 = x1 + rng.randrange(1, 7), y1 + rng.randrange(1, 7)
            px, py = rng.randrange(-6, 8), rng.randrange(-6, 8)
            queries.append(f"CONTAINS 4 {x1} {y1} {x2} {y1} {x2} {y2} {x1} {y2} {px} {py}")
    write_case(out_dir, 1, queries)


if __name__ == "__main__":
    main()
