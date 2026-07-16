#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path

Edge = tuple[int, int]
Operation = tuple[str, int]
Case = tuple[int, list[Edge], list[Operation]]


def disconnected_pair_count(
    vertex_count: int,
    edges: list[Edge],
    active: list[bool],
) -> int:
    graph = [[] for _ in range(vertex_count)]
    for enabled, (left, right) in zip(active, edges):
        if enabled:
            graph[left].append(right)
            graph[right].append(left)

    connected_pairs = 0
    visited = [False] * vertex_count
    for start in range(vertex_count):
        if visited[start]:
            continue
        visited[start] = True
        stack = [start]
        component_size = 0
        while stack:
            vertex = stack.pop()
            component_size += 1
            for neighbor in graph[vertex]:
                if not visited[neighbor]:
                    visited[neighbor] = True
                    stack.append(neighbor)
        connected_pairs += component_size * (component_size - 1) // 2
    return vertex_count * (vertex_count - 1) // 2 - connected_pairs


def answers(case: Case) -> list[int]:
    vertex_count, edges, operations = case
    active = [True] * len(edges)
    result: list[int] = []
    for operation, edge in operations:
        if operation == "D":
            active[edge] = False
        else:
            result.append(disconnected_pair_count(
                vertex_count,
                edges,
                active,
            ))
    return result


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[Case] = [
        (0, [], [("Q", -1)]),
        (1, [], [("Q", -1), ("Q", -1)]),
        (
            4,
            [(0, 1), (1, 2), (2, 3)],
            [("Q", -1), ("D", 1), ("Q", -1), ("D", 0), ("Q", -1)],
        ),
    ]

    source = random.Random(41005)
    for _ in range(120):
        vertex_count = source.randint(1, 18)
        edges = [
            (vertex, source.randrange(vertex))
            for vertex in range(1, vertex_count)
        ]
        order = list(range(len(edges)))
        source.shuffle(order)
        order = order[:source.randint(0, len(order))]
        operations: list[Operation] = [("Q", -1)]
        for edge in order:
            operations.append(("D", edge))
            if source.random() < 0.8:
                operations.append(("Q", -1))
        if operations[-1][0] != "Q":
            operations.append(("Q", -1))
        cases.append((vertex_count, edges, operations))

    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for vertex_count, edges, operations in cases:
        input_lines.append(str(vertex_count))
        input_lines.extend(f"{left} {right}" for left, right in edges)
        input_lines.append(str(len(operations)))
        for operation, edge in operations:
            input_lines.append(
                operation if operation == "Q" else f"D {edge}"
            )
        output_lines.extend(map(str, answers(
            (vertex_count, edges, operations)
        )))

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
