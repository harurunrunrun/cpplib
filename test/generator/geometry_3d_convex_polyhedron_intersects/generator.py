#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


Point = tuple[int, int, int]
Face = tuple[int, int, int]
Polyhedron = tuple[list[Point], list[Face]]


def add(first: Point, second: Point) -> Point:
    return tuple(first[index] + second[index] for index in range(3))  # type: ignore[return-value]


def subtract(first: Point, second: Point) -> Point:
    return tuple(first[index] - second[index] for index in range(3))  # type: ignore[return-value]


def multiply(value: int, point: Point) -> Point:
    return tuple(value * coordinate for coordinate in point)  # type: ignore[return-value]


def dot(first: Point, second: Point) -> int:
    return sum(first[index] * second[index] for index in range(3))


def cross(first: Point, second: Point) -> Point:
    return (
        first[1] * second[2] - first[2] * second[1],
        first[2] * second[0] - first[0] * second[2],
        first[0] * second[1] - first[1] * second[0],
    )


def determinant(first: Point, second: Point, third: Point) -> int:
    return dot(first, cross(second, third))


def box(center: Point, basis: tuple[Point, Point, Point]) -> Polyhedron:
    vertices: list[Point] = []
    for mask in range(8):
        point = center
        for axis in range(3):
            point = add(
                point,
                multiply(1 if mask & (1 << axis) else -1, basis[axis]),
            )
        vertices.append(point)

    faces: list[Face] = []
    for fixed_axis in range(3):
        varying = [axis for axis in range(3) if axis != fixed_axis]
        for side in range(2):
            masks: list[int] = []
            for first_bit, second_bit in ((0, 0), (1, 0), (1, 1), (0, 1)):
                mask = side << fixed_axis
                mask |= first_bit << varying[0]
                mask |= second_bit << varying[1]
                masks.append(mask)
            faces.append((masks[0], masks[1], masks[2]))
            faces.append((masks[0], masks[2], masks[3]))
    return vertices, faces


def tetrahedron(origin: Point, basis: tuple[Point, Point, Point]) -> Polyhedron:
    vertices = [
        origin,
        add(origin, basis[0]),
        add(origin, basis[1]),
        add(origin, basis[2]),
    ]
    return vertices, [
        (0, 2, 1),
        (0, 1, 3),
        (0, 3, 2),
        (1, 2, 3),
    ]


def edge_directions(polyhedron: Polyhedron) -> list[Point]:
    vertices, faces = polyhedron
    edges: set[tuple[int, int]] = set()
    for face in faces:
        for index in range(3):
            first, second = face[index], face[(index + 1) % 3]
            edges.add((min(first, second), max(first, second)))
    return [
        subtract(vertices[second], vertices[first])
        for first, second in sorted(edges)
    ]


def exact_intersects(first: Polyhedron, second: Polyhedron) -> bool:
    first_vertices, first_faces = first
    second_vertices, second_faces = second
    axes: list[Point] = []
    for vertices, faces in (first, second):
        for face in faces:
            axes.append(cross(
                subtract(vertices[face[1]], vertices[face[0]]),
                subtract(vertices[face[2]], vertices[face[0]]),
            ))
    for first_edge in edge_directions(first):
        for second_edge in edge_directions(second):
            axes.append(cross(first_edge, second_edge))

    for axis in axes:
        if axis == (0, 0, 0):
            continue
        first_projection = [dot(axis, point) for point in first_vertices]
        second_projection = [dot(axis, point) for point in second_vertices]
        if max(first_projection) < min(second_projection):
            return False
        if max(second_projection) < min(first_projection):
            return False
    return True


def random_basis(rng: random.Random, limit: int = 6) -> tuple[Point, Point, Point]:
    while True:
        basis = tuple(
            tuple(rng.randint(-limit, limit) for _ in range(3))
            for _ in range(3)
        )
        typed = basis  # Keep the tuple shape visible to type checkers.
        if abs(determinant(typed[0], typed[1], typed[2])) >= 4:
            return typed  # type: ignore[return-value]


def polyhedron_tokens(dimension: int, polyhedron: Polyhedron) -> list[str]:
    vertices, faces = polyhedron
    tokens = [str(dimension), str(len(vertices)), str(len(faces))]
    for point in vertices:
        tokens.extend(map(str, point))
    for face in faces:
        tokens.extend(map(str, face))
    return tokens


def both_query(first: Polyhedron, second: Polyhedron) -> tuple[str, str]:
    line = " ".join([
        "B",
        *polyhedron_tokens(3, first),
        *polyhedron_tokens(3, second),
    ])
    answer = "1" if exact_intersects(first, second) else "0"
    return line, f"{answer} {answer}"


def gjk_query(
    first_dimension: int,
    first: Polyhedron,
    second_dimension: int,
    second: Polyhedron,
    answer: bool,
) -> tuple[str, str]:
    line = " ".join([
        "G",
        *polyhedron_tokens(first_dimension, first),
        *polyhedron_tokens(second_dimension, second),
    ])
    return line, "1" if answer else "0"


def write_case(
    out_dir: Path,
    name: str,
    queries: list[tuple[str, str]],
) -> None:
    (out_dir / f"{name}.in").write_text(
        "\n".join([str(len(queries)), *(query for query, _ in queries)]) + "\n",
        encoding="utf-8",
    )
    (out_dir / f"{name}.out").write_text(
        "\n".join(answer for _, answer in queries) + "\n",
        encoding="utf-8",
    )


def fixed_cases(out_dir: Path) -> None:
    unit = ((1, 0, 0), (0, 1, 0), (0, 0, 1))
    first = box((0, 0, 0), unit)
    skew = ((2, 1, 0), (-1, 2, 1), (1, 0, 2))
    queries = [
        both_query(first, first),
        both_query(first, box((2, 0, 0), unit)),
        both_query(first, box((2, 2, 2), unit)),
        both_query(first, box((3, 0, 0), unit)),
        both_query(box((4, -3, 2), skew), box((6, -1, 1), skew)),
        both_query(box((4, -3, 2), skew), box((40, -30, 20), skew)),
        both_query(
            box((10**15, -10**15, 10**15), skew),
            box((10**15 + 4, -10**15 + 1, 10**15 - 2), skew),
        ),
        both_query(
            tetrahedron((0, 0, 0), ((4, 0, 0), (0, 5, 0), (0, 0, 6))),
            tetrahedron((1, 1, 1), ((3, 1, 0), (0, 4, 1), (1, 0, 3))),
        ),
        both_query(
            tetrahedron((0, 0, 0), ((4, 0, 0), (0, 5, 0), (0, 0, 6))),
            tetrahedron((20, 20, 20), ((3, 1, 0), (0, 4, 1), (1, 0, 3))),
        ),
    ]

    point = ([(0, 0, 0)], [])
    other_point = ([(7, -2, 3)], [])
    segment = ([(-2, 0, 0), (2, 0, 0)], [])
    crossing = ([(0, -2, 0), (0, 2, 0)], [])
    remote_segment = ([(0, -2, 1), (0, 2, 1)], [])
    queries.extend([
        gjk_query(0, point, 0, point, True),
        gjk_query(0, point, 0, other_point, False),
        gjk_query(1, segment, 1, crossing, True),
        gjk_query(1, segment, 1, remote_segment, False),
        ("S 200000", "0"),
    ])
    write_case(out_dir, "fixed_and_stress", queries)


def randomized_cases(out_dir: Path) -> None:
    for case_index, seed in enumerate((20260722, 0x9E3779B97F4A7C15, 0xD1B54A32D192ED03)):
        rng = random.Random(seed)
        queries: list[tuple[str, str]] = []
        for iteration in range(48):
            first_basis = random_basis(rng)
            second_basis = random_basis(rng)
            first_center = tuple(rng.randint(-20, 20) for _ in range(3))
            if iteration % 4 == 0:
                second_center = first_center
            elif iteration % 4 == 1:
                second_center = tuple(
                    first_center[index] + rng.choice((-1, 1)) * rng.randint(25, 45)
                    for index in range(3)
                )
            else:
                second_center = tuple(rng.randint(-24, 24) for _ in range(3))
            first = box(first_center, first_basis)  # type: ignore[arg-type]
            second = box(second_center, second_basis)  # type: ignore[arg-type]
            queries.append(both_query(first, second))

            if iteration % 6 == 0:
                first_tetrahedron = tetrahedron(
                    first_center, random_basis(rng)
                )  # type: ignore[arg-type]
                second_tetrahedron = tetrahedron(
                    second_center, random_basis(rng)
                )  # type: ignore[arg-type]
                queries.append(both_query(first_tetrahedron, second_tetrahedron))
        write_case(out_dir, f"random_{case_index:02d}", queries)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)
    fixed_cases(args.out_dir)
    randomized_cases(args.out_dir)


if __name__ == "__main__":
    main()
