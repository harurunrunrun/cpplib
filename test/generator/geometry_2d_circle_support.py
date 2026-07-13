#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import math
from pathlib import Path


def fixed(value: float) -> str:
    if abs(value) < 0.5e-12:
        value = 0.0
    return f"{value:.9f}"


def write_case(out_dir: Path, input_lines: list[str], output_lines: list[str]) -> None:
    out_dir.mkdir(parents=True, exist_ok=True)
    (out_dir / "case_00.in").write_text(
        "\n".join(input_lines) + "\n", encoding="utf-8"
    )
    (out_dir / "case_00.out").write_text(
        "\n".join(output_lines) + "\n", encoding="utf-8"
    )


def relation(first: tuple[float, float, float],
             second: tuple[float, float, float]) -> int:
    distance = math.hypot(first[0] - second[0], first[1] - second[1])
    radius_sum = first[2] + second[2]
    radius_difference = abs(first[2] - second[2])
    epsilon = 1e-10
    sign = lambda value: int(value > epsilon) - int(value < -epsilon)
    if sign(distance) == 0 and sign(radius_difference) == 0:
        return 5
    if sign(distance - radius_sum) > 0:
        return 4
    if sign(distance - radius_sum) == 0:
        return 3
    if sign(distance - radius_difference) == 0:
        return 1
    if sign(distance - radius_difference) < 0:
        return 0
    return 2


def circle_area(first: tuple[float, float, float],
                second: tuple[float, float, float]) -> float:
    first_radius = first[2]
    second_radius = second[2]
    smaller = min(first_radius, second_radius)
    if smaller == 0:
        return 0.0
    distance = math.hypot(first[0] - second[0], first[1] - second[1])
    if distance >= first_radius + second_radius:
        return 0.0
    if distance == 0 or distance <= abs(first_radius - second_radius):
        return math.pi * smaller * smaller
    first_cosine = (
        distance * distance + first_radius * first_radius - second_radius * second_radius
    ) / (2 * distance * first_radius)
    second_cosine = (
        distance * distance + second_radius * second_radius - first_radius * first_radius
    ) / (2 * distance * second_radius)
    segment = lambda radius, cosine: radius * radius * (
        math.acos(max(-1.0, min(1.0, cosine)))
        - cosine * math.sqrt(max(0.0, 1.0 - cosine * cosine))
    )
    return segment(first_radius, first_cosine) + segment(second_radius, second_cosine)


def simple_cases(name: str) -> tuple[list[str], list[str]]:
    if name == "geometry_2d_validate_circle":
        radii = [-1.0, 0.0, 2.5, -1e-12, 1e30]
        return [str(len(radii)), *(repr(value) for value in radii)], [
            "INVALID" if value < 0 else "OK" for value in radii
        ]

    circles = [
        ((0.0, 0.0, 5.0), (0.0, 0.0, 5.0)),
        ((0.0, 0.0, 5.0), (2.0, 0.0, 2.0)),
        ((0.0, 0.0, 5.0), (3.0, 0.0, 2.0)),
        ((0.0, 0.0, 5.0), (8.0, 0.0, 5.0)),
        ((0.0, 0.0, 5.0), (10.0, 0.0, 5.0)),
        ((0.0, 0.0, 5.0), (11.0, 0.0, 5.0)),
    ]
    if name in {"geometry_2d_circle_relation", "geometry_2d_intersect_circle_circle"}:
        input_lines = [str(len(circles))]
        input_lines.extend(
            " ".join(map(str, (*first, *second))) for first, second in circles
        )
        if name.endswith("circle_relation"):
            output = [str(relation(first, second)) for first, second in circles]
        else:
            output = [
                str(int(relation(first, second) not in {0, 4}))
                for first, second in circles
            ]
        return input_lines, output

    if name == "geometry_2d_circle_circle_intersection_area":
        area_cases = [
            ((0.0, 0.0, 2.0), (10.0, 0.0, 3.0)),
            ((0.0, 0.0, 2.0), (0.0, 0.0, 2.0)),
            ((0.0, 0.0, 5.0), (1.0, 0.0, 2.0)),
            ((0.0, 0.0, 5.0), (8.0, 0.0, 5.0)),
            ((-1.0, 2.0, 4.0), (2.0, -2.0, 3.0)),
        ]
        lines = [str(len(area_cases))]
        lines.extend(" ".join(map(str, (*a, *b))) for a, b in area_cases)
        return lines, [fixed(circle_area(a, b)) for a, b in area_cases]

    if name in {"geometry_2d_intersect_circle_line", "geometry_2d_intersect_circle_segment"}:
        cases = [
            (0, 0, 5, -10, 0, 10, 0),
            (0, 0, 5, -10, 5, 10, 5),
            (0, 0, 5, -10, 6, 10, 6),
            (0, 0, 5, 6, 0, 10, 0),
            (0, 0, 5, 1, 1, 1, 1),
        ]
        if name.endswith("circle_line"):
            output = ["1", "1", "0", "1", "ERROR"]
        else:
            output = ["1", "1", "0", "0", "0"]
        return [str(len(cases)), *(" ".join(map(str, case)) for case in cases)], output

    if name == "geometry_2d_circle_line_cross_points":
        cases = [
            (0, 0, 5, -10, 0, 10, 0),
            (0, 0, 5, -10, 5, 10, 5),
            (0, 0, 5, -10, 6, 10, 6),
            (0, 0, 5, 3, -10, 3, 10),
        ]
        output = [
            "2 -5.000000000 0.000000000 5.000000000 0.000000000",
            "1 0.000000000 5.000000000",
            "0",
            "2 3.000000000 -4.000000000 3.000000000 4.000000000",
        ]
        return [str(len(cases)), *(" ".join(map(str, case)) for case in cases)], output

    if name == "geometry_2d_circle_segment_cross_points":
        cases = [
            (0, 0, 5, -10, 0, 10, 0),
            (0, 0, 5, 0, 0, 10, 0),
            (0, 0, 5, -10, 5, 10, 5),
            (0, 0, 5, 5, 0, 5, 0),
            (0, 0, 5, 0, 0, 0, 0),
        ]
        output = [
            "2 -5.000000000 0.000000000 5.000000000 0.000000000",
            "1 5.000000000 0.000000000",
            "1 0.000000000 5.000000000",
            "1 5.000000000 0.000000000",
            "0",
        ]
        return [str(len(cases)), *(" ".join(map(str, case)) for case in cases)], output

    if name == "geometry_2d_circle_circle_cross_points":
        cases = [
            (0, 0, 5, 8, 0, 5),
            (0, 0, 5, 10, 0, 5),
            (0, 0, 5, 11, 0, 5),
            (0, 0, 5, 3, 0, 2),
            (0, 0, 5, 0, 0, 5),
        ]
        output = [
            "2 4.000000000 -3.000000000 4.000000000 3.000000000",
            "1 5.000000000 0.000000000",
            "0",
            "1 5.000000000 0.000000000",
            "ERROR",
        ]
        return [str(len(cases)), *(" ".join(map(str, case)) for case in cases)], output

    if name == "geometry_2d_tangent_points":
        cases = [(0, 0, 5, 0, 0), (0, 0, 5, 5, 0), (0, 0, 5, 10, 0)]
        root = math.sqrt(75.0) / 2.0
        output = [
            "0",
            "1 5.000000000 0.000000000",
            f"2 2.500000000 {fixed(-root)} 2.500000000 {fixed(root)}",
        ]
        return [str(len(cases)), *(" ".join(map(str, case)) for case in cases)], output

    if name in {"geometry_2d_tangent_lines", "geometry_2d_common_tangents"}:
        if name.endswith("tangent_lines"):
            cases = [(0, 0, 5, 0, 0), (0, 0, 5, 5, 0), (0, 0, 5, 10, 0)]
            output = ["0", "1", "2"]
        else:
            cases = [
                (0, 0, 2, 10, 0, 2),
                (0, 0, 2, 4, 0, 2),
                (0, 0, 3, 4, 0, 3),
                (0, 0, 3, 2, 0, 1),
                (0, 0, 3, 0, 0, 1),
                (0, 0, 3, 0, 0, 3),
            ]
            output = ["4", "3", "2", "1", "0", "ERROR"]
        return [str(len(cases)), *(" ".join(map(str, case)) for case in cases)], output

    if name in {
        "geometry_2d_circumcenter", "geometry_2d_circumcircle",
        "geometry_2d_incenter", "geometry_2d_incircle",
    }:
        triangles = [(0, 0, 6, 0, 0, 8), (-2, 1, 4, 1, -2, 9)]
        lines = [str(len(triangles)), *(" ".join(map(str, case)) for case in triangles)]
        centers = [(3.0, 4.0, 5.0), (1.0, 5.0, 5.0)]
        incenters = [(2.0, 2.0, 2.0), (0.0, 3.0, 2.0)]
        values = centers if "circum" in name else incenters
        if name.endswith("center"):
            return lines, [f"{fixed(x)} {fixed(y)}" for x, y, _ in values]
        return lines, [f"{fixed(x)} {fixed(y)} {fixed(radius)}" for x, y, radius in values]

    if name in {"geometry_2d_closest_pair", "geometry_2d_convex_diameter"}:
        point_sets = [
            [(0, 0), (10, 0), (3, 5), (2, 5)],
            [(-2, -1), (2, -1), (2, 2), (-2, 2)],
            [(7, 9)],
        ]
        input_lines = [str(len(point_sets))]
        for points in point_sets:
            input_lines.append(str(len(points)))
            input_lines.extend(f"{x} {y}" for x, y in points)
        if name.endswith("closest_pair"):
            output = ["1.000000000", "3.000000000", "INF"]
        else:
            output = ["10.000000000", "5.000000000", "0.000000000"]
        return input_lines, output

    raise ValueError(f"unknown geometry test: {name}")


def run(name: str) -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    input_lines, output_lines = simple_cases(name)
    write_case(Path(args.out_dir), input_lines, output_lines)
