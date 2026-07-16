#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import collections
import random
from pathlib import Path


def path_edges(
    graph: list[list[int]],
    source: int,
    target: int,
) -> list[tuple[int, int]]:
    parent = [-1] * len(graph)
    parent[source] = source
    queue = collections.deque([source])
    while queue:
        vertex = queue.popleft()
        for next_vertex in graph[vertex]:
            if parent[next_vertex] == -1:
                parent[next_vertex] = vertex
                queue.append(next_vertex)
    result = []
    while target != source:
        result.append(tuple(sorted((target, parent[target]))))
        target = parent[target]
    return result


def build_dataset(
    source: random.Random,
    vertex_count: int,
    operation_count: int,
) -> tuple[list[str], list[str]]:
    edges = [
        (source.randrange(vertex), vertex)
        for vertex in range(1, vertex_count)
    ]
    graph = [[] for _ in range(vertex_count)]
    for left, right in edges:
        graph[left].append(right)
        graph[right].append(left)
    value = {tuple(sorted(edge)): 0 for edge in edges}
    root = source.randrange(vertex_count)
    operations: list[str] = []
    output: list[str] = []

    for _ in range(operation_count):
        if vertex_count == 1 or source.randrange(3) != 0:
            left = source.randrange(vertex_count)
            right = source.randrange(vertex_count)
            delta = source.randint(-100, 100)
            operations.append(f"A {left} {right} {delta}")
            for edge in path_edges(graph, left, right):
                value[edge] += delta
        else:
            left, right = source.choice(edges)
            if source.randrange(2):
                left, right = right, left
            operations.append(f"G {left} {right}")
            output.append(str(value[tuple(sorted((left, right)))]))

    return [
        f"{vertex_count} {len(operations)} {root}",
        *(f"{left} {right}" for left, right in edges),
        *operations,
    ], output


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    source = random.Random(31006)
    datasets = [build_dataset(source, 1, 30)]
    datasets.extend(
        build_dataset(source, source.randint(2, 50), 250)
        for _ in range(25)
    )
    input_lines = [str(len(datasets))]
    output_lines: list[str] = []
    for lines, answers in datasets:
        input_lines.extend(lines)
        output_lines.extend(answers)

    (out_dir / "case_00.in").write_text(
        "\n".join(input_lines) + "\n", encoding="ascii"
    )
    (out_dir / "case_00.out").write_text(
        "\n".join(output_lines) + "\n", encoding="ascii"
    )


if __name__ == "__main__":
    main()
