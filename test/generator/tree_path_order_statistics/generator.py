#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from dataclasses import dataclass
from pathlib import Path


Operation = tuple[str, int, int, int, int | None]


@dataclass
class Case:
    values: list[int]
    edges: list[tuple[int, int]]
    root: int
    operations: list[Operation]


def path_values(case: Case, source: int, target: int) -> list[int]:
    graph = [[] for _ in case.values]
    for left, right in case.edges:
        graph[left].append(right)
        graph[right].append(left)
    parent = [-1] * len(graph)
    stack = [source]
    parent[source] = source
    while stack:
        vertex = stack.pop()
        if vertex == target:
            break
        for to in graph[vertex]:
            if parent[to] == -1:
                parent[to] = vertex
                stack.append(to)
    vertices = []
    vertex = target
    while vertex != source:
        vertices.append(vertex)
        vertex = parent[vertex]
    vertices.append(source)
    return [case.values[vertex] for vertex in vertices]


def expected(case: Case) -> list[str]:
    output: list[str] = []
    for operation, left, right, first, second in case.operations:
        values = path_values(case, left, right)
        if operation == "K":
            output.append(str(sorted(values)[first]))
        elif operation == "G":
            output.append(str(sorted(values, reverse=True)[first]))
        elif operation == "C":
            output.append(str(sum(value < first for value in values)))
        elif operation == "E":
            output.append(str(sum(value == first for value in values)))
        elif operation == "R":
            assert second is not None
            output.append(str(sum(first <= value < second for value in values)))
        else:
            output.append(str(len(values)))
    return output


def random_case(rng: random.Random, size: int, query_count: int) -> Case:
    values = [rng.randint(-12, 12) for _ in range(size)]
    edges = [(vertex, rng.randrange(vertex)) for vertex in range(1, size)]
    rng.shuffle(edges)
    case = Case(values, edges, rng.randrange(size), [])
    for _ in range(query_count):
        left = rng.randrange(size)
        right = rng.randrange(size)
        path = path_values(case, left, right)
        operation = rng.choice(("K", "G", "C", "E", "R", "S"))
        if operation in ("K", "G"):
            case.operations.append((operation, left, right, rng.randrange(len(path)), None))
        elif operation in ("C", "E"):
            case.operations.append((operation, left, right, rng.randint(-16, 16), None))
        elif operation == "R":
            lower = rng.randint(-16, 16)
            upper = rng.randint(lower, 17)
            case.operations.append((operation, left, right, lower, upper))
        else:
            case.operations.append((operation, left, right, 0, None))
    return case


def maximum_size_case() -> Case:
    size = 256
    values = [((index * 73) % size) - size // 2 for index in range(size)]
    operations: list[Operation] = [
        ("K", 0, size - 1, order, None) for order in range(size)
    ]
    operations.extend(
        ("G", 0, size - 1, order, None) for order in range(size)
    )
    for threshold in (-200, -128, -127, -1, 0, 1, 127, 128, 200):
        operations.append(("C", 0, size - 1, threshold, None))
        operations.append(("E", 0, size - 1, threshold, None))
    for lower, upper in (
        (-200, 200),
        (-128, 128),
        (-127, 127),
        (-1, -1),
        (-1, 0),
        (0, 1),
        (127, 128),
    ):
        operations.append(("R", 0, size - 1, lower, upper))
    operations.append(("S", 0, size - 1, 0, None))
    return Case(
        values,
        [(vertex - 1, vertex) for vertex in range(1, size)],
        size - 1,
        operations,
    )


def integer_boundary_case() -> Case:
    minimum = -(1 << 63)
    maximum = (1 << 63) - 1
    values = [minimum, -1, 0, 1, maximum, minimum, maximum]
    return Case(
        values,
        [(0, vertex) for vertex in range(1, len(values))],
        4,
        [
            ("K", 5, 6, 0, None),
            ("K", 5, 6, 2, None),
            ("G", 5, 6, 0, None),
            ("C", 5, 6, minimum, None),
            ("C", 5, 6, maximum, None),
            ("E", 5, 6, minimum, None),
            ("E", 5, 6, maximum, None),
            ("R", 5, 6, minimum, maximum),
            ("R", 5, 6, maximum, maximum),
            ("S", 5, 6, 0, None),
        ],
    )


def write_cases(out_dir: Path, name: str, cases: list[Case]) -> None:
    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for case in cases:
        input_lines.append(
            f"{len(case.values)} {len(case.operations)} {case.root}"
        )
        input_lines.append(" ".join(map(str, case.values)))
        input_lines.extend(f"{left} {right}" for left, right in case.edges)
        for operation, left, right, first, second in case.operations:
            line = f"{operation} {left} {right}"
            if operation in ("K", "G", "C", "E"):
                line += f" {first}"
            elif operation == "R":
                line += f" {first} {second}"
            input_lines.append(line)
        output_lines.extend(expected(case))

    (out_dir / f"{name}.in").write_text(
        "\n".join(input_lines) + "\n", encoding="utf-8"
    )
    (out_dir / f"{name}.out").write_text(
        "\n".join(output_lines) + "\n", encoding="utf-8"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    manual_cases = [
        Case(
            [4],
            [],
            0,
            [("K", 0, 0, 0, None), ("C", 0, 0, 4, None), ("S", 0, 0, 0, None)],
        ),
        Case(
            [5, -2, 5, 7, -2, 9],
            [(0, 1), (1, 2), (1, 3), (3, 4), (3, 5)],
            3,
            [
                ("K", 2, 4, 0, None),
                ("K", 2, 4, 3, None),
                ("G", 2, 4, 1, None),
                ("E", 2, 4, -2, None),
                ("R", 2, 4, -2, 6),
            ],
        ),
        Case(
            [105, 2, 9, 3, 8, 5, 7, 7],
            [(0, 1), (0, 2), (0, 3), (2, 4), (2, 5), (2, 6), (3, 7)],
            0,
            [
                ("K", 1, 4, 0, None),
                ("K", 1, 4, 1, None),
                ("K", 1, 4, 2, None),
                ("K", 1, 4, 3, None),
                ("K", 6, 7, 1, None),
            ],
        ),
    ]
    rng = random.Random(20260714)
    small_random_cases = [
        random_case(rng, size, 80 + size) for size in range(2, 25)
    ]
    large_random_cases = [
        random_case(rng, size, 80 + size) for size in (31, 63, 127, 200, 256)
    ]

    write_cases(out_dir, "case_00_manual", manual_cases)
    write_cases(out_dir, "case_01_small_random", small_random_cases)
    write_cases(out_dir, "case_02_large_random", large_random_cases)
    write_cases(out_dir, "case_03_maximum_size", [maximum_size_case()])
    write_cases(out_dir, "case_04_integer_boundary", [integer_boundary_case()])


if __name__ == "__main__":
    main()
