#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


Point = tuple[int, int]


def cross(first: Point, second: Point, third: Point) -> int:
    return ((second[0] - first[0]) * (third[1] - second[1])
            - (second[1] - first[1]) * (third[0] - second[0]))


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
    polygons: list[tuple[list[Point], bool]] = [
        ([(0, 0), (4, 0), (4, 3), (0, 3)], True),
        ([(0, 3), (4, 3), (4, 0), (0, 0)], True),
        ([(0, 0), (5, 0), (5, 4), (3, 2), (0, 4)], True),
        ([(0, 0), (2, 0), (4, 0), (4, 3), (0, 3)], True),
        ([(0, 0), (0, 0), (4, 0), (4, 3), (0, 3), (0, 0)], True),
        ([(0, 0), (4, 4), (0, 4), (4, 0)], False),
        ([(0, 0), (4, 0), (4, 3), (0, 0), (0, 3)], False),
        ([(0, 0), (1, 0), (2, 0)], False),
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
        polygons.append((polygon, True))

    input_data = [str(len(polygons))]
    output_data: list[str] = []
    for polygon, valid in polygons:
        input_data.append(polygon_input(polygon).rstrip())
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
