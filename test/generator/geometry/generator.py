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


def line_intersection(nums: list[int]) -> tuple[int, int]:
    ax, ay, bx, by, cx, cy, dx, dy = nums
    rx, ry = bx - ax, by - ay
    sx, sy = dx - cx, dy - cy
    denominator = cross((rx, ry), (sx, sy))
    numerator = cross((cx - ax, cy - ay), (sx, sy))
    assert denominator != 0
    assert (ax * denominator + rx * numerator) % denominator == 0
    assert (ay * denominator + ry * numerator) % denominator == 0
    return (
        (ax * denominator + rx * numerator) // denominator,
        (ay * denominator + ry * numerator) // denominator,
    )


def is_convex(poly: list[Point]) -> int:
    direction = 0
    n = len(poly)
    for i in range(n):
        turn = sign(cross(sub(poly[(i + 1) % n], poly[i]), sub(poly[(i + 2) % n], poly[(i + 1) % n])))
        if turn == 0:
            continue
        if direction == 0:
            direction = turn
        elif direction != turn:
            return 0
    return 1


def diameter_squared(poly: list[Point]) -> int:
    return max(
        (poly[i][0] - poly[j][0]) ** 2 + (poly[i][1] - poly[j][1]) ** 2
        for i in range(len(poly))
        for j in range(len(poly))
    ) if poly else 0


def closest_pair_squared(points: list[Point]) -> int:
    return min(
        (points[i][0] - points[j][0]) ** 2 + (points[i][1] - points[j][1]) ** 2
        for i in range(len(points))
        for j in range(i)
    )

def circle_relation(x1: int, y1: int, r1: int, x2: int, y2: int, r2: int) -> int:
    d2 = (x1 - x2) ** 2 + (y1 - y2) ** 2
    rsum = r1 + r2
    rdiff = abs(r1 - r2)
    if d2 == 0 and r1 == r2:
        return 5
    if d2 > rsum * rsum:
        return 4
    if d2 == rsum * rsum:
        return 3
    if d2 == rdiff * rdiff:
        return 1
    if d2 < rdiff * rdiff:
        return 0
    return 2


def circle_line_count(nums: list[int]) -> int:
    cx, cy, r, ax, ay, bx, by = nums
    dx, dy = bx - ax, by - ay
    num = abs(cross((dx, dy), (cx - ax, cy - ay)))
    den2 = dx * dx + dy * dy
    lhs = num * num
    rhs = r * r * den2
    if lhs > rhs:
        return 0
    if lhs == rhs:
        return 1
    return 2


def circle_circle_count(nums: list[int]) -> int:
    relation = circle_relation(*nums)
    if relation == 5:
        return -1
    if relation == 2:
        return 2
    if relation in (1, 3):
        return 1
    return 0


def common_tangent_count(nums: list[int]) -> int:
    relation = circle_relation(*nums)
    if relation == 5:
        return -1
    return [0, 1, 2, 3, 4][relation]


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
        elif xs[0] == "LINE_INTERSECTION":
            point = line_intersection(list(map(int, xs[1:])))
            out.append(f"{point[0]} {point[1]}")
        elif xs[0] in {"CONTAINS", "CONTAINS_CONVEX"}:
            n = int(xs[1])
            nums = list(map(int, xs[2:]))
            poly = [(nums[2 * i], nums[2 * i + 1]) for i in range(n)]
            p = (nums[2 * n], nums[2 * n + 1])
            out.append(str(contains(poly, p)))
        elif xs[0] in {"IS_CONVEX", "CONVEX_DIAMETER2", "CLOSEST_PAIR2"}:
            n = int(xs[1])
            nums = list(map(int, xs[2:]))
            poly = [(nums[2 * i], nums[2 * i + 1]) for i in range(n)]
            if xs[0] == "IS_CONVEX":
                out.append(str(is_convex(poly)))
            elif xs[0] == "CONVEX_DIAMETER2":
                out.append(str(diameter_squared(poly)))
            else:
                out.append(str(closest_pair_squared(poly)))
        elif xs[0] == "CIRCLE_RELATION":
            out.append(str(circle_relation(*map(int, xs[1:]))))
        elif xs[0] == "CIRCLE_LINE_COUNT":
            out.append(str(circle_line_count(list(map(int, xs[1:])))))
        elif xs[0] == "CIRCLE_CIRCLE_COUNT":
            out.append(str(circle_circle_count(list(map(int, xs[1:])))))
        elif xs[0] == "COMMON_TANGENT_COUNT":
            out.append(str(common_tangent_count(list(map(int, xs[1:])))))
    return "\n".join(out) + "\n"


def write_case(out_dir: Path, idx: int, queries: list[str]) -> None:
    name = f"case_{idx:02d}"
    data = "\n".join([str(len(queries)), *queries]) + "\n"
    (out_dir / f"{name}.in").write_text(data, encoding="utf-8")
    (out_dir / f"{name}.out").write_text(solve(queries), encoding="utf-8")


def write_convex_query_case(out_dir: Path) -> None:
    radius = 20_000
    top = radius * radius + 4 * radius
    polygon = [(x, x * x) for x in range(-radius, radius + 1)]
    polygon.extend([(radius, top), (-radius, top)])

    points: list[Point] = []
    answers: list[int] = []
    for index in range(60_000):
        x = -radius + 1 + index % (2 * radius - 1)
        kind = index % 6
        if kind == 0:
            point, answer = (x, x * x), 1
        elif kind == 1:
            point, answer = (x, x * x + 1), 2
        elif kind == 2:
            point, answer = (x, x * x - 1), 0
        elif kind == 3:
            point, answer = (x, top), 1
        elif kind == 4:
            point, answer = (x, top + 1), 0
        else:
            point, answer = (x, (x * x + top) // 2), 2
        points.append(point)
        answers.append(answer)

    input_lines = ["1", f"CONVEX_QUERY {len(polygon)} {len(points)}"]
    input_lines.extend(f"{x} {y}" for x, y in polygon)
    input_lines.extend(f"{x} {y}" for x, y in points)
    (out_dir / "case_03.in").write_text("\n".join(input_lines) + "\n", encoding="utf-8")
    (out_dir / "case_03.out").write_text(
        " ".join(map(str, [len(answers), *answers])) + "\n",
        encoding="utf-8",
    )


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
        "INTERSECT 0 0 0 0 1 0 1 0",
        "INTERSECT 0 0 0 0 0 0 0 0",
        "LINE_INTERSECTION 0 0 1 0 2 -1 2 1",
        "CONTAINS 4 0 0 4 0 4 4 0 4 2 2",
        "CONTAINS 4 0 0 4 0 4 4 0 4 4 2",
        "CONTAINS 4 0 0 4 0 4 4 0 4 5 2",
        "CONTAINS_CONVEX 4 0 0 4 0 4 4 0 4 2 2",
        "CONTAINS_CONVEX 4 0 4 4 4 4 0 0 0 2 2",
        "CONTAINS_CONVEX 8 0 0 0 0 2 0 4 0 4 2 4 4 0 4 0 2 3 0",
        "IS_CONVEX 4 0 4 4 4 4 0 0 0",
        "IS_CONVEX 5 0 0 2 0 1 1 2 2 0 2",
        "CONVEX_DIAMETER2 4 0 0 4 0 4 4 0 4",
        "CONVEX_DIAMETER2 9 0 0 2 0 2 0 4 0 4 2 4 4 2 4 0 4 0 2",
        "CONVEX_DIAMETER2 8 0 2 0 4 2 4 4 4 4 2 4 0 2 0 0 0",
        "CLOSEST_PAIR2 5 0 0 10 0 3 4 3 5 0 0",
        "CIRCLE_RELATION 0 0 2 5 0 1",
        "CIRCLE_RELATION 0 0 2 3 0 1",
        "CIRCLE_RELATION 0 0 2 2 0 2",
        "CIRCLE_RELATION 0 0 3 2 0 1",
        "CIRCLE_RELATION 0 0 4 1 0 1",
        "CIRCLE_RELATION 1 2 3 1 2 3",
        "CIRCLE_LINE_COUNT 0 0 5 -10 0 10 0",
        "CIRCLE_LINE_COUNT 0 0 5 -10 5 10 5",
        "CIRCLE_CIRCLE_COUNT 0 0 5 8 0 5",
        "CIRCLE_CIRCLE_COUNT 0 0 5 0 0 5",
        "COMMON_TANGENT_COUNT 0 0 1 4 0 1",
        "COMMON_TANGENT_COUNT 0 0 1 0 0 1",
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

    queries = []
    for _ in range(80):
        kind = rng.randrange(5)
        if kind == 0:
            qx, qy = rng.randrange(-20, 21), rng.randrange(-20, 21)
            while True:
                rx, ry = rng.randrange(-5, 6), rng.randrange(-5, 6)
                sx, sy = rng.randrange(-5, 6), rng.randrange(-5, 6)
                if (rx or ry) and (sx or sy) and cross((rx, ry), (sx, sy)) != 0:
                    break
            queries.append(f"LINE_INTERSECTION {qx-rx} {qy-ry} {qx+rx} {qy+ry} {qx-sx} {qy-sy} {qx+sx} {qy+sy}")
        elif kind == 1:
            x1, y1 = rng.randrange(-20, 0), rng.randrange(-20, 0)
            x2, y2 = rng.randrange(1, 21), rng.randrange(1, 21)
            if rng.randrange(2):
                queries.append(f"CONVEX_DIAMETER2 4 {x1} {y1} {x2} {y1} {x2} {y2} {x1} {y2}")
            else:
                queries.append(f"IS_CONVEX 4 {x1} {y2} {x2} {y2} {x2} {y1} {x1} {y1}")
        elif kind == 2:
            points = [(rng.randrange(-10, 11), rng.randrange(-10, 11)) for _ in range(12)]
            queries.append("CLOSEST_PAIR2 12 " + " ".join(f"{x} {y}" for x, y in points))
        elif kind == 3:
            nums = [rng.randrange(-5, 6), rng.randrange(-5, 6), rng.randrange(0, 6), rng.randrange(-5, 6), rng.randrange(-5, 6), rng.randrange(0, 6)]
            queries.append("CIRCLE_RELATION " + " ".join(map(str, nums)))
        else:
            nums = [rng.randrange(-5, 6), rng.randrange(-5, 6), rng.randrange(1, 6), rng.randrange(-5, 6), rng.randrange(-5, 6), rng.randrange(1, 6)]
            queries.append("COMMON_TANGENT_COUNT " + " ".join(map(str, nums)))
    write_case(out_dir, 2, queries)
    write_convex_query_case(out_dir)


if __name__ == "__main__":
    main()