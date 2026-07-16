#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def rooted_subtrees(
    vertex_count: int,
    edges: list[tuple[int, int]],
    root: int,
) -> list[list[int]]:
    graph = [[] for _ in range(vertex_count)]
    for left, right in edges:
        graph[left].append(right)
        graph[right].append(left)
    parent = [-1] * vertex_count
    parent[root] = root
    order = [root]
    for vertex in order:
        for next_vertex in graph[vertex]:
            if parent[next_vertex] == -1:
                parent[next_vertex] = vertex
                order.append(next_vertex)
    children = [[] for _ in range(vertex_count)]
    for vertex in range(vertex_count):
        if vertex != root:
            children[parent[vertex]].append(vertex)
    result: list[list[int]] = []
    for start in range(vertex_count):
        members = []
        stack = [start]
        while stack:
            vertex = stack.pop()
            members.append(vertex)
            stack.extend(children[vertex])
        result.append(members)
    return result



def build_case(
    source: random.Random,
    vertex_count: int,
    operation_count: int,
) -> tuple[list[str], list[str]]:
    edges = [
        (source.randrange(vertex), vertex)
        for vertex in range(1, vertex_count)
    ]
    root = source.randrange(vertex_count)
    subtrees = rooted_subtrees(vertex_count, edges, root)
    initial = [source.randint(-50, 80) for _ in range(vertex_count)]
    values = initial[:]
    operations: list[str] = []
    output: list[str] = []
    for _ in range(operation_count):
        vertex = source.randrange(vertex_count)
        operation = source.choice("SSMMAACC")
        members = subtrees[vertex]
        if operation == "S":
            operations.append(f"S {vertex}")
            output.append(str(sum(values[index] for index in members)))
        elif operation == "M":
            operations.append(f"M {vertex}")
            output.append(str(min(values[index] for index in members)))
        elif operation == "A":
            delta = source.randint(-30, 30)
            operations.append(f"A {vertex} {delta}")
            for index in members:
                values[index] += delta
        else:
            cap = source.randint(-20, 50)
            delta = min(cap, min(values[index] for index in members))
            operations.append(f"C {vertex} {cap}")
            output.append(str(delta))
            for index in members:
                values[index] += delta
    lines = [
        f"{vertex_count} {len(operations)} {root}",
        " ".join(map(str, initial)),
        *(f"{left} {right}" for left, right in edges),
        *operations,
    ]
    return lines, output


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    source = random.Random(31002)
    datasets = [build_case(source, 1, 40)]
    datasets.extend(
        build_case(source, source.randint(2, 35), 140)
        for _ in range(20)
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
