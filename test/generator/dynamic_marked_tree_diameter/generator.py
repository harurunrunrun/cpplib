#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from dataclasses import dataclass
from pathlib import Path


@dataclass
class Case:
    size: int
    edges: list[tuple[int, int, int]]
    initially_marked: bool
    operations: list[tuple[str, int | None]]


def all_distances(case: Case) -> list[list[int]]:
    graph: list[list[tuple[int, int]]] = [[] for _ in range(case.size)]
    for left, right, weight in case.edges:
        graph[left].append((right, weight))
        graph[right].append((left, weight))
    result = [[0] * case.size for _ in range(case.size)]
    for source in range(case.size):
        stack = [(source, -1, 0)]
        while stack:
            vertex, parent, distance = stack.pop()
            result[source][vertex] = distance
            for to, weight in graph[vertex]:
                if to != parent:
                    stack.append((to, vertex, distance + weight))
    return result


def expected(case: Case) -> list[str]:
    distances = all_distances(case)
    marked = [case.initially_marked] * case.size
    output: list[str] = []
    for operation, argument in case.operations:
        if operation == "Q":
            vertices = [vertex for vertex, value in enumerate(marked) if value]
            if not vertices:
                output.append("EMPTY")
            else:
                answer = 0
                for left in vertices:
                    for right in vertices:
                        answer = max(answer, distances[left][right])
                output.append(str(answer))
            continue
        assert argument is not None
        if operation == "M":
            changed = not marked[argument]
            marked[argument] = True
            output.append(str(int(changed)))
        elif operation == "U":
            changed = marked[argument]
            marked[argument] = False
            output.append(str(int(changed)))
        else:
            marked[argument] = not marked[argument]
            output.append(str(int(marked[argument])))
    return output


def random_case(rng: random.Random, size: int, operation_count: int) -> Case:
    edges = []
    for vertex in range(1, size):
        edges.append((vertex, rng.randrange(vertex), rng.randint(-30, 30)))
    rng.shuffle(edges)
    operations: list[tuple[str, int | None]] = []
    for _ in range(operation_count):
        operation = rng.choices(["M", "U", "T", "Q"], [2, 2, 3, 4])[0]
        operations.append((operation, None if operation == "Q" else rng.randrange(size)))
    operations.extend([("Q", None), ("T", rng.randrange(size)), ("Q", None)])
    return Case(size, edges, bool(rng.randrange(2)), operations)


def write_cases(path: Path, cases: list[Case]) -> None:
    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for case in cases:
        input_lines.append(
            f"{case.size} {int(case.initially_marked)} {len(case.operations)}"
        )
        input_lines.extend(f"{u} {v} {w}" for u, v, w in case.edges)
        for operation, argument in case.operations:
            input_lines.append(operation if argument is None else f"{operation} {argument}")
        output_lines.extend(expected(case))
    path.with_suffix(".in").write_text("\n".join(input_lines) + "\n", encoding="utf-8")
    path.with_suffix(".out").write_text("\n".join(output_lines) + "\n", encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    fixed = [
        Case(
            1,
            [],
            False,
            [("Q", None), ("M", 0), ("M", 0), ("Q", None),
             ("U", 0), ("U", 0), ("Q", None), ("T", 0), ("Q", None)],
        ),
        Case(
            4,
            [(0, 1, -5), (1, 2, -2), (2, 3, -9)],
            True,
            [("Q", None), ("U", 1), ("Q", None), ("T", 2),
             ("Q", None), ("U", 0), ("U", 3), ("Q", None)],
        ),
        Case(
            6,
            [(0, 1, 8), (1, 2, -12), (1, 3, 7), (3, 4, 5), (3, 5, -20)],
            False,
            [("M", 2), ("M", 4), ("Q", None), ("M", 0), ("Q", None),
             ("T", 2), ("Q", None), ("T", 5), ("Q", None)],
        ),
        Case(
            8,
            [(0, vertex, (-1 if vertex % 2 else 1) * vertex) for vertex in range(1, 8)],
            True,
            [("Q", None), ("T", 0), ("Q", None), ("T", 7),
             ("Q", None), ("M", 0), ("Q", None)],
        ),
    ]
    write_cases(out_dir / "case_00", fixed)

    rng = random.Random(20260714)
    for file_index, sizes in enumerate(([2, 3, 5, 9, 17], [7, 15, 31, 63], [4, 12, 40, 80]), 1):
        cases = [random_case(rng, size, 100 + size * 3) for size in sizes]
        write_cases(out_dir / f"case_{file_index:02d}", cases)


if __name__ == "__main__":
    main()
