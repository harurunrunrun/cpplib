#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from fractions import Fraction
from pathlib import Path


IntPoint = tuple[int, int]
RatPoint = tuple[Fraction, Fraction]
SCALE = 1_000_000


def sub(a: RatPoint, b: RatPoint) -> RatPoint:
    return (a[0] - b[0], a[1] - b[1])


def add(a: RatPoint, b: RatPoint) -> RatPoint:
    return (a[0] + b[0], a[1] + b[1])


def mul(a: RatPoint, value: Fraction) -> RatPoint:
    return (a[0] * value, a[1] * value)


def cross(a: RatPoint, b: RatPoint) -> Fraction:
    return a[0] * b[1] - a[1] * b[0]


def convex_hull(points: list[RatPoint]) -> list[RatPoint]:
    points = sorted(set(points))
    if len(points) <= 1:
        return points

    lower: list[RatPoint] = []
    for point in points:
        while len(lower) >= 2 and cross(
            sub(lower[-1], lower[-2]),
            sub(point, lower[-1]),
        ) <= 0:
            lower.pop()
        lower.append(point)

    upper: list[RatPoint] = []
    for point in reversed(points):
        while len(upper) >= 2 and cross(
            sub(upper[-1], upper[-2]),
            sub(point, upper[-1]),
        ) <= 0:
            upper.pop()
        upper.append(point)
    return lower[:-1] + upper[:-1]


def as_rat(polygon: list[IntPoint]) -> list[RatPoint]:
    return [(Fraction(x), Fraction(y)) for x, y in polygon]


def line_cross_point(
    first: RatPoint,
    second: RatPoint,
    clip_first: RatPoint,
    clip_second: RatPoint,
) -> RatPoint:
    direction = sub(second, first)
    clip_direction = sub(clip_second, clip_first)
    denominator = cross(direction, clip_direction)
    assert denominator != 0
    parameter = cross(sub(clip_first, first), clip_direction) / denominator
    return add(first, mul(direction, parameter))


def convex_intersection(first: list[IntPoint], second: list[IntPoint]) -> list[RatPoint]:
    subject = convex_hull(as_rat(first))
    clip = convex_hull(as_rat(second))
    if not subject or not clip:
        return []
    if len(subject) < 3 or len(clip) < 3:
        raise ValueError("the generic oracle expects full-dimensional polygons")

    for index, clip_first in enumerate(clip):
        clip_second = clip[(index + 1) % len(clip)]
        output: list[RatPoint] = []
        if not subject:
            break
        for subject_index, current in enumerate(subject):
            previous = subject[subject_index - 1]
            current_inside = cross(
                sub(clip_second, clip_first),
                sub(current, clip_first),
            ) >= 0
            previous_inside = cross(
                sub(clip_second, clip_first),
                sub(previous, clip_first),
            ) >= 0
            if current_inside:
                if not previous_inside:
                    output.append(line_cross_point(
                        previous,
                        current,
                        clip_first,
                        clip_second,
                    ))
                output.append(current)
            elif previous_inside:
                output.append(line_cross_point(
                    previous,
                    current,
                    clip_first,
                    clip_second,
                ))
        subject = output
    return convex_hull(subject)


def rounded_scaled(value: Fraction) -> int:
    numerator = value.numerator * SCALE
    denominator = value.denominator
    if numerator >= 0:
        return (2 * numerator + denominator) // (2 * denominator)
    return -((-2 * numerator + denominator) // (2 * denominator))


def polygon_answer(polygon: list[RatPoint], prefix: str = "P") -> str:
    polygon = convex_hull(polygon)
    tokens = [prefix, str(len(polygon))]
    for x, y in polygon:
        tokens.extend((str(rounded_scaled(x)), str(rounded_scaled(y))))
    return " ".join(tokens)


def polygon_tokens(polygon: list[IntPoint]) -> list[str]:
    tokens = [str(len(polygon))]
    for x, y in polygon:
        tokens.extend((str(x), str(y)))
    return tokens


def polygon_query(operation: str, first: list[IntPoint], second: list[IntPoint]) -> str:
    return " ".join([operation, *polygon_tokens(first), *polygon_tokens(second)])


def half_plane_query(lines: list[tuple[IntPoint, IntPoint]]) -> str:
    tokens = ["H", str(len(lines))]
    for (ax, ay), (bx, by) in lines:
        tokens.extend((str(ax), str(ay), str(bx), str(by)))
    return " ".join(tokens)


def edge_lines(polygon: list[IntPoint]) -> list[tuple[IntPoint, IntPoint]]:
    return [
        (polygon[index], polygon[(index + 1) % len(polygon)])
        for index in range(len(polygon))
    ]


def decorate(polygon: list[IntPoint], rng: random.Random) -> list[IntPoint]:
    result: list[IntPoint] = []
    for point in polygon:
        result.append(point)
        if rng.randrange(4) == 0:
            result.append(point)
    if result and rng.randrange(2):
        result.append(result[0])
    if rng.randrange(2):
        result.reverse()
    return result


def random_hull(rng: random.Random, count: int = 12) -> list[IntPoint]:
    while True:
        points = [
            (rng.randint(-30, 30), rng.randint(-30, 30))
            for _ in range(count)
        ]
        hull = convex_hull(as_rat(points))
        if len(hull) >= 3:
            return [(int(x), int(y)) for x, y in hull]


def write_case(
    out_dir: Path,
    index: int,
    queries: list[str],
    answers: list[str],
) -> None:
    assert len(queries) == len(answers)
    stem = f"case_{index:02d}"
    (out_dir / f"{stem}.in").write_text(
        "\n".join([str(len(queries)), *queries]) + "\n",
        encoding="utf-8",
    )
    (out_dir / f"{stem}.out").write_text(
        "\n".join(answers) + "\n",
        encoding="utf-8",
    )


def fixed_cases(out_dir: Path) -> None:
    queries: list[str] = []
    answers: list[str] = []

    def add_hpi(lines: list[tuple[IntPoint, IntPoint]], answer: str) -> None:
        queries.append(half_plane_query(lines))
        answers.append(answer)

    add_hpi([], "U")
    add_hpi([((0, 0), (1, 0))], "U")
    add_hpi([((0, 0), (1, 0)), ((1, 1), (0, 1))], "U")
    add_hpi([((0, 0), (1, 0)), ((0, 0), (0, -1))], "U")
    add_hpi([((0, 1), (1, 1)), ((1, 0), (0, 0))], "E")

    square = [(0, 0), (4, 0), (4, 4), (0, 4)]
    square_lines = edge_lines(square)
    square_lines.extend([
        ((0, -3), (4, -3)),
        ((0, 0), (4, 0)),
    ])
    random.Random(91).shuffle(square_lines)
    add_hpi(square_lines, polygon_answer(as_rat(square), "B"))

    segment_lines = [
        ((0, 1), (0, 0)),
        ((0, 0), (0, 1)),
        ((0, 0), (1, 0)),
        ((1, 2), (0, 2)),
    ]
    add_hpi(segment_lines, polygon_answer(as_rat([(0, 0), (0, 2)]), "B"))
    add_hpi(
        [*segment_lines, ((1, 0), (0, 0))],
        polygon_answer(as_rat([(0, 0)]), "B"),
    )

    first = [(0, 0), (4, 0), (4, 4), (0, 4)]
    second = [(0, 0), (2, 0), (2, 2), (0, 2)]
    queries.append(polygon_query("M", decorate(first, random.Random(1)), decorate(second, random.Random(2))))
    pair_sums = [(ax + bx, ay + by) for ax, ay in first for bx, by in second]
    answers.append(polygon_answer(as_rat(pair_sums)))

    queries.append(polygon_query("M", [], second))
    answers.append("P 0")
    queries.append(polygon_query("M", [(10, -3)], [(0, 0), (4, 0)]))
    answers.append(polygon_answer(as_rat([(10, -3), (14, -3)])))

    overlap_first = [(0, 0), (5, 0), (5, 4), (0, 4)]
    overlap_second = [(2, -1), (7, -1), (7, 3), (2, 3)]
    queries.append(polygon_query("I", overlap_first, list(reversed(overlap_second))))
    answers.append(polygon_answer(convex_intersection(overlap_first, overlap_second)))

    touching = [(4, 1), (7, 1), (7, 3), (4, 3)]
    queries.append(polygon_query("I", first, touching))
    answers.append(polygon_answer(as_rat([(4, 1), (4, 3)])))
    queries.append(polygon_query("I", [(-1, 1), (5, 1)], first))
    answers.append(polygon_answer(as_rat([(0, 1), (4, 1)])))
    queries.append(polygon_query("I", [(2, 2)], first))
    answers.append(polygon_answer(as_rat([(2, 2)])))
    queries.append(polygon_query("I", [(6, 6)], first))
    answers.append("P 0")
    queries.append(polygon_query("I", [(0, 0), (4, 0)], [(2, 0), (6, 0)]))
    answers.append(polygon_answer(as_rat([(2, 0), (4, 0)])))

    large_base = 10**12
    large_square = [
        (large_base, -large_base),
        (large_base + 100, -large_base),
        (large_base + 100, -large_base + 100),
        (large_base, -large_base + 100),
    ]
    add_hpi(edge_lines(large_square), polygon_answer(as_rat(large_square), "B"))
    queries.append(polygon_query("I", large_square, large_square))
    answers.append(polygon_answer(as_rat(large_square)))
    write_case(out_dir, 0, queries, answers)


def random_cases(out_dir: Path) -> None:
    rng = random.Random(20260713)
    queries: list[str] = []
    answers: list[str] = []
    for _ in range(120):
        first = random_hull(rng)
        second = random_hull(rng)
        decorated_first = decorate(first, rng)
        decorated_second = decorate(second, rng)

        queries.append(polygon_query("M", decorated_first, decorated_second))
        sums = [(ax + bx, ay + by) for ax, ay in first for bx, by in second]
        answers.append(polygon_answer(as_rat(sums)))

        queries.append(polygon_query("I", decorated_first, decorated_second))
        answers.append(polygon_answer(convex_intersection(first, second)))

        lines = edge_lines(first)
        for start, end in edge_lines(first)[::2]:
            dx = end[0] - start[0]
            dy = end[1] - start[1]
            shift = (dy, -dx)
            lines.append((
                (start[0] + shift[0], start[1] + shift[1]),
                (end[0] + shift[0], end[1] + shift[1]),
            ))
        rng.shuffle(lines)
        queries.append(half_plane_query(lines))
        answers.append(polygon_answer(as_rat(first), "B"))

        missing_edge = edge_lines(first)[:2]
        rng.shuffle(missing_edge)
        queries.append(half_plane_query(missing_edge))
        answers.append("U")

        start, end = rng.choice(edge_lines(first))
        dx = end[0] - start[0]
        dy = end[1] - start[1]

        left_shift = (-dy, dx)
        strip_lines = [
            (start, end),
            (
                (end[0] + left_shift[0], end[1] + left_shift[1]),
                (start[0] + left_shift[0], start[1] + left_shift[1]),
            ),
        ]
        rng.shuffle(strip_lines)
        queries.append(half_plane_query(strip_lines))
        answers.append("U")

        right_shift = (dy, -dx)
        contradictory_lines = [
            (start, end),
            (
                (end[0] + right_shift[0], end[1] + right_shift[1]),
                (start[0] + right_shift[0], start[1] + right_shift[1]),
            ),
        ]
        rng.shuffle(contradictory_lines)
        queries.append(half_plane_query(contradictory_lines))
        answers.append("E")
    write_case(out_dir, 1, queries, answers)


def large_case(out_dir: Path) -> None:
    radius = 10_000
    top = radius * radius + 4 * radius
    polygon = [(x, x * x) for x in range(-radius, radius + 1)]
    polygon.extend([(radius, top), (-radius, top)])
    doubled = [(2 * x, 2 * y) for x, y in polygon]

    queries = [
        half_plane_query(edge_lines(polygon)),
        polygon_query("M", polygon, polygon),
        polygon_query("I", polygon, polygon),
    ]
    answers = [
        polygon_answer(as_rat(polygon), "B"),
        polygon_answer(as_rat(doubled)),
        polygon_answer(as_rat(polygon)),
    ]
    write_case(out_dir, 2, queries, answers)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)
    fixed_cases(args.out_dir)
    random_cases(args.out_dir)
    large_case(args.out_dir)


if __name__ == "__main__":
    main()
