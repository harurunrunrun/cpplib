#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


Point = tuple[int, int]
Segment = tuple[Point, Point]


def cross(first: Point, second: Point, third: Point) -> int:
    return ((second[0] - first[0]) * (third[1] - first[1])
            - (second[1] - first[1]) * (third[0] - first[0]))


def on_segment(first: Point, second: Point, point: Point) -> bool:
    return (cross(first, second, point) == 0
            and min(first[0], second[0]) <= point[0] <= max(first[0], second[0])
            and min(first[1], second[1]) <= point[1] <= max(first[1], second[1]))


def intersection_kind(first: Segment, second: Segment) -> str | None:
    a, b = first
    c, d = second
    orientations = (
        cross(a, b, c), cross(a, b, d),
        cross(c, d, a), cross(c, d, b),
    )
    if all(value == 0 for value in orientations):
        lower = max(min(a, b), min(c, d))
        upper = min(max(a, b), max(c, d))
        if lower > upper:
            return None
        return "P" if lower == upper else "S"
    if orientations[0] == 0 and on_segment(a, b, c):
        return "P"
    if orientations[1] == 0 and on_segment(a, b, d):
        return "P"
    if orientations[2] == 0 and on_segment(c, d, a):
        return "P"
    if orientations[3] == 0 and on_segment(c, d, b):
        return "P"
    if (orientations[0] * orientations[1] < 0
            and orientations[2] * orientations[3] < 0):
        return "P"
    return None


def brute(segments: list[Segment]) -> list[tuple[int, int, str]]:
    result: list[tuple[int, int, str]] = []
    for first in range(len(segments)):
        for second in range(first + 1, len(segments)):
            kind = intersection_kind(segments[first], segments[second])
            if kind is not None:
                result.append((first, second, kind))
    return result


def append_query(lines: list[str], segments: list[Segment]) -> None:
    lines.append(str(len(segments)))
    lines.extend(
        f"{a[0]} {a[1]} {b[0]} {b[1]}" for a, b in segments
    )


def deterministic_cases() -> list[list[Segment]]:
    return [
        [],
        [((0, 0), (1, 1))],
        [((0, -3), (0, 3)), ((-4, 0), (4, 0))],
        [
            ((0, 0), (5, 5)), ((0, 5), (5, 0)),
            ((2, -3), (2, 8)), ((-4, 2), (8, 2)),
        ],
        [
            ((0, 0), (6, 0)), ((2, 0), (8, 0)),
            ((3, 0), (3, 0)), ((6, 0), (9, 0)),
        ],
        [
            ((0, 0), (0, 8)), ((0, 2), (0, 4)),
            ((0, 4), (0, 10)), ((0, 5), (0, 5)),
        ],
        [
            ((-5, -5), (5, 5)), ((-5, 5), (5, -5)),
            ((-5, 0), (5, 0)), ((0, -5), (0, 5)),
            ((0, 0), (0, 0)),
        ],
        [
            ((0, 0), (3, 0)), ((3, 0), (3, 3)),
            ((3, 3), (0, 3)), ((0, 3), (0, 0)),
        ],
    ]


def random_cases() -> list[list[Segment]]:
    generator = random.Random(0xB0171E)
    cases: list[list[Segment]] = []
    for _ in range(450):
        count = generator.randint(0, 14)
        segments: list[Segment] = []
        for _ in range(count):
            a = (generator.randint(-8, 8), generator.randint(-8, 8))
            if generator.randrange(7) == 0:
                b = a
            elif generator.randrange(5) == 0:
                b = (a[0], generator.randint(-8, 8))
            elif generator.randrange(5) == 0:
                b = (generator.randint(-8, 8), a[1])
            else:
                b = (generator.randint(-8, 8), generator.randint(-8, 8))
            segments.append((a, b))
        cases.append(segments)
    return cases


def render(cases: list[list[Segment]], mode: str) -> tuple[str, str]:
    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for segments in cases:
        append_query(input_lines, segments)
        answer = brute(segments)
        if mode == "enumerate":
            output_lines.append(str(len(answer)))
            output_lines.extend(f"{a} {b} {kind}" for a, b, kind in answer)
        elif mode == "find":
            output_lines.append("1" if answer else "0")
        elif mode == "count":
            output_lines.append(str(len(answer)))
        else:
            raise ValueError(mode)
    return "\n".join(input_lines) + "\n", "\n".join(output_lines) + "\n"


def write_cases(cases: list[tuple[str, str]]) -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    for index, (input_data, output_data) in enumerate(cases):
        stem = f"case_{index:02d}"
        (out_dir / f"{stem}.in").write_text(input_data, encoding="utf-8")
        (out_dir / f"{stem}.out").write_text(output_data, encoding="utf-8")


def generate_sweep_cases(mode: str) -> None:
    small = render(deterministic_cases() + random_cases(), mode)
    maximum_segments = [
        ((0, 3 * index), (100, 3 * index + 1)) for index in range(20_000)
    ]
    maximum_input = ["1"]
    append_query(maximum_input, maximum_segments)
    maximum = ("\n".join(maximum_input) + "\n", "0\n")
    write_cases([small, maximum])
