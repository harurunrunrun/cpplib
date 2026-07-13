#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import random
from pathlib import Path


Point = tuple[int, int]


def cross(origin: Point, first: Point, second: Point) -> int:
    return ((first[0] - origin[0]) * (second[1] - origin[1])
            - (first[1] - origin[1]) * (second[0] - origin[0]))


def convex_hull(points: list[Point]) -> list[Point]:
    points = sorted(set(points))
    if len(points) <= 1:
        return points

    lower: list[Point] = []
    for point in points:
        while len(lower) >= 2 and cross(lower[-2], lower[-1], point) <= 0:
            lower.pop()
        lower.append(point)
    upper: list[Point] = []
    for point in reversed(points):
        while len(upper) >= 2 and cross(upper[-2], upper[-1], point) <= 0:
            upper.pop()
        upper.append(point)
    hull = lower[:-1] + upper[:-1]
    if len(hull) == 2 and hull[1] < hull[0]:
        hull.reverse()
    return hull


def weak_polygon(hull: list[Point], rng: random.Random) -> list[Point]:
    if len(hull) <= 1:
        result = hull.copy()
        if result and rng.randrange(2):
            result.extend([result[0], result[0]])
        return result
    if len(hull) == 2:
        first, second = hull
        result = [first]
        if (first[0] + second[0]) % 2 == 0 and (first[1] + second[1]) % 2 == 0:
            result.append(((first[0] + second[0]) // 2,
                           (first[1] + second[1]) // 2))
        result.extend([second, second, first])
        if rng.randrange(2):
            result.reverse()
        return result

    result: list[Point] = []
    for index, point in enumerate(hull):
        result.append(point)
        if rng.randrange(4) == 0:
            result.append(point)
        next_point = hull[(index + 1) % len(hull)]
        if (rng.randrange(3) == 0
                and (point[0] + next_point[0]) % 2 == 0
                and (point[1] + next_point[1]) % 2 == 0):
            result.append(((point[0] + next_point[0]) // 2,
                           (point[1] + next_point[1]) // 2))
    shift = rng.randrange(len(result))
    result = result[shift:] + result[:shift]
    if rng.randrange(2):
        result.reverse()
    if rng.randrange(2):
        result.append(result[0])
    return result


def random_hull(rng: random.Random, maximum_points: int = 28) -> list[Point]:
    count = rng.randrange(maximum_points + 1)
    points = [
        (rng.randrange(-80, 81), rng.randrange(-80, 81))
        for _ in range(count)
    ]
    return convex_hull(points)


def lens_polygon(half_width: int, height: int) -> list[Point]:
    lower = [(x, x * x) for x in range(-half_width, half_width + 1)]
    upper = [
        (x, height - x * x)
        for x in range(half_width, -half_width - 1, -1)
    ]
    return lower + upper


def encode_polygon(polygon: list[Point]) -> str:
    coordinates = [str(coordinate) for point in polygon for coordinate in point]
    return " ".join([str(len(polygon)), *coordinates])


def squared_distance(first: Point, second: Point) -> int:
    return ((first[0] - second[0]) ** 2
            + (first[1] - second[1]) ** 2)


def brute_diameter(hull: list[Point]) -> int:
    return max(
        (squared_distance(first, second) for first in hull for second in hull),
        default=0,
    )


def brute_farthest_distances(hull: list[Point]) -> list[int]:
    return [
        max(squared_distance(point, other) for other in hull)
        for point in hull
    ]


def brute_maximum_triangle(hull: list[Point]) -> tuple[bool, int]:
    if len(hull) < 3:
        return False, 0
    best = 0
    for first in range(len(hull)):
        for second in range(first + 1, len(hull)):
            for third in range(second + 1, len(hull)):
                best = max(best, abs(cross(
                    hull[first], hull[second], hull[third]
                )))
    return True, best


def calipers_maximum_triangle(hull: list[Point]) -> int:
    best = 0
    count = len(hull)
    for first in range(count - 2):
        third = first + 2
        for second in range(first + 1, count - 1):
            third = max(third, second + 1)
            while third + 1 < count:
                current = abs(cross(hull[first], hull[second], hull[third]))
                following = abs(cross(hull[first], hull[second], hull[third + 1]))
                if following < current:
                    break
                third += 1
            best = max(best, abs(cross(hull[first], hull[second], hull[third])))
    return best


def support_vertex(hull: list[Point], direction: Point) -> tuple[int, int]:
    if not hull:
        return -1, 0
    values = [point[0] * direction[0] + point[1] * direction[1]
              for point in hull]
    best = max(values)
    return values.index(best), best


def brute_tangents(hull: list[Point], point: Point) -> tuple[int, int]:
    if not hull:
        return -1, -1
    left: list[int] = []
    right: list[int] = []
    for index, vertex in enumerate(hull):
        sides = [cross(point, vertex, other) for other in hull]
        if all(side >= 0 for side in sides):
            left.append(index)
        if all(side <= 0 for side in sides):
            right.append(index)
    assert left and right
    return min(left), min(right)


def linear_tangents(hull: list[Point], point: Point) -> tuple[int, int]:
    if not hull:
        return -1, -1
    if len(hull) <= 2:
        return brute_tangents(hull, point)
    left = [
        index for index in range(len(hull))
        if cross(point, hull[index], hull[index - 1]) >= 0
        and cross(point, hull[index], hull[(index + 1) % len(hull)]) >= 0
    ]
    right = [
        index for index in range(len(hull))
        if cross(point, hull[index], hull[index - 1]) <= 0
        and cross(point, hull[index], hull[(index + 1) % len(hull)]) <= 0
    ]
    assert left and right
    return min(left), min(right)


def write_case(
    out_dir: Path,
    index: int,
    queries: list[str],
    answers: list[str],
) -> None:
    assert len(queries) == len(answers)
    stem = f"case_{index:02d}"
    (out_dir / f"{stem}.in").write_text(
        "\n".join([str(len(queries)), *queries]) + "\n", encoding="utf-8"
    )
    (out_dir / f"{stem}.out").write_text(
        "\n".join(answers) + "\n", encoding="utf-8"
    )
