#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from fractions import Fraction
from pathlib import Path
import random
import sys

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "advanced_common"))
from generator_utils import generate

CASES = [("8\n4\n0 0\n3 0\n3 3\n0 3\n4\n1 -1\n2 -1\n2 4\n1 4\n4\n0 0\n1 0\n1 1\n0 1\n4\n2 0\n3 0\n3 1\n2 1\n2\n0 0\n2 0\n2\n1 0\n3 0\n2\n-1 1\n3 1\n4\n0 0\n2 0\n2 2\n0 2\n1\n1 1\n4\n0 0\n2 0\n2 2\n0 2\n1\n3 3\n4\n0 0\n2 0\n2 2\n0 2\n5\n0 0\n2 0\n1 1\n2 2\n0 2\n0\n4\n0 0\n2 0\n2 2\n0 2\n0\n", "4 1000000 0 2000000 0 2000000 3000000 1000000 3000000\n0\n2 1000000 0 2000000 0\n2 0 1000000 2000000 1000000\n1 1000000 1000000\n0\nERR\n0\n")]


def cross(a, b, c):
    return (b[0] - a[0]) * (c[1] - a[1]) - (b[1] - a[1]) * (c[0] - a[0])


def convex_hull(points):
    points = sorted(set(points))
    if len(points) <= 1:
        return points
    lower = []
    for point in points:
        while len(lower) >= 2 and cross(lower[-2], lower[-1], point) <= 0:
            lower.pop()
        lower.append(point)
    upper = []
    for point in reversed(points):
        while len(upper) >= 2 and cross(upper[-2], upper[-1], point) <= 0:
            upper.pop()
        upper.append(point)
    return lower[:-1] + upper[:-1]


def line_intersection(segment_start, segment_end, clip_start, clip_end):
    start_side = cross(clip_start, clip_end, segment_start)
    end_side = cross(clip_start, clip_end, segment_end)
    denominator = start_side - end_side
    return (
        segment_start[0] + (segment_end[0] - segment_start[0])
        * Fraction(start_side, denominator),
        segment_start[1] + (segment_end[1] - segment_start[1])
        * Fraction(start_side, denominator),
    )


def normalize(polygon):
    compact = []
    for point in polygon:
        if not compact or compact[-1] != point:
            compact.append(point)
    if len(compact) >= 2 and compact[0] == compact[-1]:
        compact.pop()
    if len(compact) >= 3 and all(
        cross(compact[0], compact[1], point) == 0
        for point in compact[2:]
    ):
        minimum, maximum = min(compact), max(compact)
        return [minimum] if minimum == maximum else [minimum, maximum]
    changed = True
    while changed and len(compact) >= 3:
        changed = False
        kept = []
        count = len(compact)
        for index, point in enumerate(compact):
            if cross(compact[index - 1], point, compact[(index + 1) % count]) == 0:
                changed = True
            else:
                kept.append(point)
        compact = kept
    if len(compact) <= 1:
        return compact
    if len(compact) == 2:
        return sorted(compact)
    area2 = sum(
        compact[index][0] * compact[(index + 1) % len(compact)][1]
        - compact[index][1] * compact[(index + 1) % len(compact)][0]
        for index in range(len(compact))
    )
    if area2 < 0:
        compact.reverse()
    start = min(range(len(compact)), key=compact.__getitem__)
    return compact[start:] + compact[:start]


def reference_intersection(first, second):
    subject = [(Fraction(x), Fraction(y)) for x, y in first]
    clip = [(Fraction(x), Fraction(y)) for x, y in second]
    for index, clip_start in enumerate(clip):
        clip_end = clip[(index + 1) % len(clip)]
        if not subject:
            break
        output = []
        previous = subject[-1]
        previous_inside = cross(clip_start, clip_end, previous) >= 0
        for current in subject:
            current_inside = cross(clip_start, clip_end, current) >= 0
            if current_inside != previous_inside:
                output.append(line_intersection(previous, current, clip_start, clip_end))
            if current_inside:
                output.append(current)
            previous = current
            previous_inside = current_inside
        subject = output
    return normalize(subject)


def round_scaled(value):
    scaled = value * 1_000_000
    numerator = scaled.numerator
    denominator = scaled.denominator
    if numerator >= 0:
        return (2 * numerator + denominator) // (2 * denominator)
    return -((2 * (-numerator) + denominator) // (2 * denominator))


def encode_polygon(polygon):
    values = [str(len(polygon))]
    for x, y in polygon:
        values.extend((str(round_scaled(x)), str(round_scaled(y))))
    return " ".join(values)


def random_case(seed, query_count):
    rng = random.Random(seed)
    input_lines = [str(query_count)]
    output_lines = []
    for _ in range(query_count):
        first = convex_hull([
            (rng.randrange(-30, 31), rng.randrange(-30, 31))
            for _ in range(rng.randrange(6, 30))
        ])
        second = convex_hull([
            (rng.randrange(-30, 31), rng.randrange(-30, 31))
            for _ in range(rng.randrange(6, 30))
        ])
        input_lines.append(str(len(first)))
        input_lines.extend(f"{x} {y}" for x, y in first)
        input_lines.append(str(len(second)))
        input_lines.extend(f"{x} {y}" for x, y in second)
        output_lines.append(encode_polygon(reference_intersection(first, second)))
    return "\n".join(input_lines) + "\n", "\n".join(output_lines) + "\n"


def fixed_boundary_case():
    pairs = [
        (
            [(0, 0), (1, 0), (1, 1), (0, 1)],
            [(1, 0), (2, 0), (2, 1), (1, 1)],
        ),
        (
            [(0, 0), (1, 0), (1, 1), (0, 1)],
            [(1, 1), (2, 1), (2, 2), (1, 2)],
        ),
        (
            [(0, 0), (1, 0), (1, 1), (0, 1)],
            [(2, 0), (3, 0), (3, 1), (2, 1)],
        ),
        (
            [(-3, -3), (3, -3), (3, 3), (-3, 3)],
            [(-1, -2), (2, -1), (1, 2), (-2, 1)],
        ),
    ]
    input_lines = [str(len(pairs))]
    output_lines = []
    for first, second in pairs:
        input_lines.extend([str(len(first)), *[f"{x} {y}" for x, y in first]])
        input_lines.extend([str(len(second)), *[f"{x} {y}" for x, y in second]])
        output_lines.append(encode_polygon(reference_intersection(first, second)))
    return "\n".join(input_lines) + "\n", "\n".join(output_lines) + "\n"


def boundary_validation_case():
    containing = [(-10, -10), (10, -10), (10, 10), (-10, 10)]
    pairs = [
        (
            [(0, 3), (2, -3), (-3, 1), (3, 1), (-2, -3)],
            containing,
            "ERR",
        ),
        (
            [
                (0, 0), (2, 0), (2, 2), (0, 2),
                (0, 0), (2, 0), (2, 2), (0, 2),
            ],
            containing,
            "ERR",
        ),
        (
            [(0, 0), (0, 1), (0, 2), (2, 2), (2, 0), (0, 0), (0, 0)],
            containing,
            encode_polygon([(0, 0), (2, 0), (2, 2), (0, 2)]),
        ),
    ]
    input_lines = [str(len(pairs))]
    output_lines = []
    for first, second, expected in pairs:
        input_lines.extend([str(len(first)), *[f"{x} {y}" for x, y in first]])
        input_lines.extend([str(len(second)), *[f"{x} {y}" for x, y in second]])
        output_lines.append(expected)
    return "\n".join(input_lines) + "\n", "\n".join(output_lines) + "\n"


def large_identical_case(radius):
    polygon = [(x, x * x) for x in range(-radius, radius + 1)]
    input_lines = ["1", str(len(polygon))]
    input_lines.extend(f"{x} {y}" for x, y in polygon)
    input_lines.append(str(len(polygon)))
    input_lines.extend(f"{x} {y}" for x, y in polygon)
    output = encode_polygon([(Fraction(x), Fraction(y)) for x, y in polygon]) + "\n"
    return "\n".join(input_lines) + "\n", output


if __name__ == "__main__":
    CASES.append(fixed_boundary_case())
    CASES.append(boundary_validation_case())
    CASES.extend(random_case(seed, 100) for seed in (20260717, 20260718, 20260719))
    CASES.append(large_identical_case(2500))
    generate(CASES)
