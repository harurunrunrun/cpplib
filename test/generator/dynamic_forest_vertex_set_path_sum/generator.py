#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import collections
import random
from pathlib import Path

Operation = tuple[str, int, int | None]
Case = tuple[list[int], list[Operation]]


def find_path(
    graph: list[set[int]],
    start: int,
    goal: int,
) -> list[int] | None:
    parent = {start: -1}
    queue = collections.deque([start])
    while queue:
        vertex = queue.popleft()
        if vertex == goal:
            path = []
            while vertex != -1:
                path.append(vertex)
                vertex = parent[vertex]
            return path
        for neighbor in graph[vertex]:
            if neighbor not in parent:
                parent[neighbor] = vertex
                queue.append(neighbor)
    return None


def evaluate(case: Case) -> list[str]:
    initial_values, operations = case
    values = initial_values.copy()
    graph = [set() for _ in values]
    result: list[str] = []
    for operation, left, right in operations:
        if operation == "U":
            assert right is not None
            values[left] = right
        elif operation == "G":
            result.append(str(values[left]))
        elif operation == "L":
            assert right is not None
            path = find_path(graph, left, right)
            success = path is None
            if success:
                graph[left].add(right)
                graph[right].add(left)
            result.append(str(int(success)))
        elif operation == "C":
            assert right is not None
            success = right in graph[left]
            if success:
                graph[left].remove(right)
                graph[right].remove(left)
            result.append(str(int(success)))
        elif operation == "Q":
            assert right is not None
            result.append(str(int(
                find_path(graph, left, right) is not None
            )))
        else:
            assert right is not None
            path = find_path(graph, left, right)
            result.append(
                "X" if path is None
                else str(sum(values[vertex] for vertex in path))
            )
    return result


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[Case] = [
        ([], []),
        (
            [3, -2, 7, 5],
            [
                ("L", 0, 1),
                ("L", 1, 2),
                ("S", 0, 2),
                ("L", 0, 2),
                ("U", 1, 10),
                ("S", 0, 2),
                ("G", 1, None),
                ("C", 0, 2),
                ("C", 1, 2),
                ("S", 0, 2),
                ("Q", 0, 2),
            ],
        ),
    ]

    source = random.Random(41006)
    for _ in range(70):
        vertex_count = source.randint(1, 14)
        values = [
            source.randint(-100, 100)
            for _ in range(vertex_count)
        ]
        operations: list[Operation] = []
        for _ in range(source.randint(40, 100)):
            operation = source.choices(
                ["L", "C", "U", "G", "S", "Q"],
                weights=[3, 2, 2, 1, 3, 2],
            )[0]
            left = source.randrange(vertex_count)
            if operation == "U":
                operations.append((
                    operation,
                    left,
                    source.randint(-100, 100),
                ))
            elif operation == "G":
                operations.append((operation, left, None))
            else:
                operations.append((
                    operation,
                    left,
                    source.randrange(vertex_count),
                ))
        cases.append((values, operations))

    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for values, operations in cases:
        input_lines.append(f"{len(values)} {len(operations)}")
        if values:
            input_lines.append(" ".join(map(str, values)))
        for operation, left, right in operations:
            if operation == "G":
                input_lines.append(f"G {left}")
            else:
                assert right is not None
                input_lines.append(f"{operation} {left} {right}")
        output_lines.extend(evaluate((values, operations)))

    (out_dir / "case_00.in").write_text(
        "\n".join(input_lines) + "\n",
        encoding="ascii",
    )
    (out_dir / "case_00.out").write_text(
        "\n".join(output_lines) + "\n",
        encoding="ascii",
    )


if __name__ == "__main__":
    main()
