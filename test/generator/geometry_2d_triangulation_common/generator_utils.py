#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import math
import random
from pathlib import Path


Point = tuple[int, int]


def cross(first: Point, second: Point, third: Point) -> int:
    return ((second[0] - first[0]) * (third[1] - second[1])
            - (second[1] - first[1]) * (third[0] - second[0]))

def on_segment(first: Point, second: Point, point: Point) -> bool:
    return (cross(first, second, point) == 0
            and min(first[0], second[0]) <= point[0]
            <= max(first[0], second[0])
            and min(first[1], second[1]) <= point[1]
            <= max(first[1], second[1]))


def segments_intersect(
    first: Point,
    second: Point,
    third: Point,
    fourth: Point,
) -> bool:
    first_turn = cross(first, second, third)
    second_turn = cross(first, second, fourth)
    third_turn = cross(third, fourth, first)
    fourth_turn = cross(third, fourth, second)
    if first_turn == 0 and on_segment(first, second, third):
        return True
    if second_turn == 0 and on_segment(first, second, fourth):
        return True
    if third_turn == 0 and on_segment(third, fourth, first):
        return True
    if fourth_turn == 0 and on_segment(third, fourth, second):
        return True
    return ((first_turn > 0) != (second_turn > 0)
            and (third_turn > 0) != (fourth_turn > 0))


def is_simple_non_degenerate(polygon: list[Point]) -> bool:
    vertices: list[Point] = []
    for point in polygon:
        if not vertices or vertices[-1] != point:
            vertices.append(point)
    if len(vertices) >= 2 and vertices[0] == vertices[-1]:
        vertices.pop()
    count = len(vertices)
    if count < 3 or len(set(vertices)) != count:
        return False
    if sum(cross((0, 0), vertices[index],
                 vertices[(index + 1) % count])
           for index in range(count)) == 0:
        return False
    for first in range(count):
        first_start = vertices[first]
        first_end = vertices[(first + 1) % count]
        for second in range(first + 1, count):
            second_start = vertices[second]
            second_end = vertices[(second + 1) % count]
            adjacent = ((first + 1) % count == second
                        or (second + 1) % count == first)
            if not adjacent:
                if segments_intersect(
                    first_start, first_end, second_start, second_end
                ):
                    return False
                continue
            shared = second_start if (first + 1) % count == second else first_start
            first_other = first_start if shared == first_end else first_end
            second_other = second_end if shared == second_start else second_start
            if (on_segment(first_start, first_end, second_other)
                    or on_segment(second_start, second_end, first_other)):
                return False
    return normalized_size(vertices) >= 3


def normalized_size(polygon: list[Point]) -> int:
    vertices: list[Point] = []
    for point in polygon:
        if not vertices or vertices[-1] != point:
            vertices.append(point)
    if len(vertices) >= 2 and vertices[0] == vertices[-1]:
        vertices.pop()
    changed = True
    while changed and len(vertices) >= 3:
        changed = False
        for index in range(len(vertices)):
            previous = vertices[index - 1]
            point = vertices[index]
            following = vertices[(index + 1) % len(vertices)]
            if (cross(previous, point, following) == 0
                    and min(previous[0], following[0]) <= point[0]
                    <= max(previous[0], following[0])
                    and min(previous[1], following[1]) <= point[1]
                    <= max(previous[1], following[1])):
                vertices.pop(index)
                changed = True
                break
    return len(vertices)


def polygon_input(polygon: list[Point]) -> str:
    coordinates = " ".join(f"{x} {y}" for x, y in polygon)
    return f"{len(polygon)}\n{coordinates}\n"


def make_cases() -> list[tuple[str, str]]:
    polygons: list[tuple[list[Point], bool | None]] = [
        ([(0, 0), (4, 0), (4, 3), (0, 3)], None),
        ([(0, 3), (4, 3), (4, 0), (0, 0)], None),
        ([(0, 0), (5, 0), (5, 4), (3, 2), (0, 4)], None),
        ([(0, 0), (2, 0), (4, 0), (4, 3), (0, 3)], None),
        ([(0, 0), (0, 0), (4, 0), (4, 3), (0, 3), (0, 0)], None),
        ([(0, 0), (4, 4), (0, 4), (4, 0)], None),
        ([(0, 0), (4, 0), (4, 3), (0, 0), (0, 3)], None),
        ([(0, 0), (1, 0), (2, 0)], None),
        ([(0, 0), (6, 0), (6, 5), (5, 5), (5, 1),
          (4, 1), (4, 4), (3, 4), (3, 1), (2, 1),
          (2, 5), (0, 5)], None),
        ([(0, 0), (5, 0), (5, 5), (0, 5), (5, 0)], None),
        ([(0, 0), (6, 0), (6, 3), (2, 3), (4, 3), (0, 3)], None),
        ([(0, 0), (5, 0), (5, 5), (0, 5), (0, 0), (5, 5)], None),
    ]
    random_generator = random.Random(0xEA2C11)
    for _ in range(250):
        width = random_generator.randint(4, 10)
        bottom_x = sorted(random_generator.sample(
            range(1, width), random_generator.randint(1, width - 1)
        ))
        top_x = sorted(random_generator.sample(
            range(1, width), random_generator.randint(1, width - 1)
        ), reverse=True)
        polygon = [(0, 0)]
        polygon.extend(
            (x, -random_generator.randint(1, 12)) for x in bottom_x
        )
        polygon.append((width, 0))
        polygon.extend(
            (x, random_generator.randint(1, 12)) for x in top_x
        )
        polygons.append((polygon, None))

    for _ in range(300):
        count = random_generator.randint(6, 45)
        polygon: list[Point] = []
        for index in range(count):
            angle = (2.0 * math.pi
                     * (index + random_generator.uniform(0.15, 0.85))
                     / count)
            radius = random_generator.randint(30, 300)
            polygon.append((
                round(radius * math.cos(angle)),
                round(radius * math.sin(angle)),
            ))
        if len(set(polygon)) != len(polygon):
            continue
        if random_generator.randrange(2) == 0:
            polygon.reverse()
        if random_generator.randrange(5) == 0:
            position = random_generator.randrange(len(polygon))
            polygon.insert(position, polygon[position])
        if random_generator.randrange(5) == 0:
            polygon.append(polygon[0])
        polygons.append((polygon, None))

        repeated = polygon.copy()
        if repeated[-1] == repeated[0]:
            repeated.pop()
        repeated.insert(len(repeated) // 2, repeated[0])
        polygons.append((repeated, None))

    tooth_count = 5000
    width = 2 * tooth_count + 2
    comb: list[Point] = [(0, 0), (width, 0), (width, 4)]
    for x in range(width - 1, 2, -2):
        comb.extend([(x, 4), (x, 1), (x - 1, 1), (x - 1, 4)])
    comb.append((0, 4))
    polygons.append((comb, True))

    input_data = [str(len(polygons))]
    output_data: list[str] = []
    for polygon, known_validity in polygons:
        input_data.append(polygon_input(polygon).rstrip())
        valid = (is_simple_non_degenerate(polygon)
                 if known_validity is None else known_validity)
        if valid:
            output_data.append(f"{normalized_size(polygon) - 2} 1")
        else:
            output_data.append("ERROR")
    return [("\n".join(input_data) + "\n", "\n".join(output_data) + "\n")]


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


def generate_triangulation_cases() -> None:
    write_cases(make_cases())
