#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path

Point = tuple[int, int]
Query = tuple[int, int, int]


def cross(first: Point, second: Point, point: Point) -> int:
    return ((second[0] - first[0]) * (point[1] - first[1])
            - (second[1] - first[1]) * (point[0] - first[0]))


def inside(first: Point, second: Point, third: Point, point: Point) -> bool:
    values = [
        cross(first, second, point),
        cross(second, third, point),
        cross(third, first, point),
    ]
    return all(value > 0 for value in values) or all(value < 0 for value in values)


def write_case(
    out_dir: Path,
    index: int,
    vertices: list[Point],
    points: list[Point],
    queries: list[Query],
) -> None:
    with (out_dir / f"case_{index:02d}.in").open(
        "w", encoding="utf-8", newline="\n"
    ) as input_file, (out_dir / f"case_{index:02d}.out").open(
        "w", encoding="utf-8", newline="\n"
    ) as output_file:
        input_file.write(f"{len(vertices)}\n")
        input_file.writelines(f"{x} {y}\n" for x, y in vertices)
        input_file.write(f"{len(points)}\n")
        input_file.writelines(f"{x} {y}\n" for x, y in points)
        input_file.write(f"{len(queries)}\n")
        for first, second, third in queries:
            input_file.write(f"{first} {second} {third}\n")
            answer = sum(
                inside(vertices[first], vertices[second], vertices[third], point)
                for point in points
            )
            output_file.write(f"{answer}\n")


def write_max(out_dir: Path, index: int) -> None:
    vertex_count = 500
    point_count = 500
    query_count = 1_000_000
    with (out_dir / f"case_{index:02d}.in").open(
        "w", encoding="utf-8", newline="\n"
    ) as input_file, (out_dir / f"case_{index:02d}.out").open(
        "w", encoding="utf-8", newline="\n"
    ) as output_file:
        input_file.write(f"{vertex_count}\n")
        for value in range(vertex_count):
            input_file.write(f"{value} {(value * value) % 997}\n")
        input_file.write(f"{point_count}\n")
        for value in range(point_count):
            input_file.write(f"{-value} {(value * 37) % 991}\n")
        input_file.write(f"{query_count}\n")
        input_file.writelines("0 0 0\n" for _ in range(query_count))
        output_file.writelines("0\n" for _ in range(query_count))


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)

    vertices = [(0, 0), (6, 0), (0, 6), (6, 6), (3, 3)]
    points = [(1, 1), (2, 2), (3, 1), (0, 0), (3, 0), (5, 5), (7, 7)]
    queries = [(0, 1, 2), (0, 3, 1), (0, 4, 3), (0, 0, 1), (2, 1, 0)]
    write_case(args.out_dir, 0, vertices, points, queries)

    source = random.Random(2026071319)
    for case in range(1, 9):
        vertex_count = source.randint(1, 18)
        point_count = source.randint(1, 22)
        random_vertices = [
            (source.randint(-20, 20), source.randint(-20, 20))
            for _ in range(vertex_count)
        ]
        random_points = [
            (source.randint(-20, 20), source.randint(-20, 20))
            for _ in range(point_count)
        ]
        random_queries = [
            (
                source.randrange(vertex_count),
                source.randrange(vertex_count),
                source.randrange(vertex_count),
            )
            for _ in range(300)
        ]
        write_case(
            args.out_dir,
            case,
            random_vertices,
            random_points,
            random_queries,
        )
    write_max(args.out_dir, 9)


if __name__ == "__main__":
    main()
