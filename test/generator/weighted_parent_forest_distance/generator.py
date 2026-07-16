#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def root_of(vertex: int, parent: list[int]) -> int:
    while parent[vertex] != vertex:
        vertex = parent[vertex]
    return vertex


def distance_to_root(
    vertex: int,
    parent: list[int],
    weight: list[int],
) -> int:
    result = 0
    while parent[vertex] != vertex:
        result += weight[vertex]
        vertex = parent[vertex]
    return result


def build_dataset(
    source: random.Random,
    vertex_count: int,
) -> tuple[list[str], list[str]]:
    parent = list(range(vertex_count))
    weight = [0] * vertex_count
    roots = set(range(vertex_count))
    operations: list[str] = []
    output: list[str] = []

    for _ in range(max(30, vertex_count * 8)):
        if len(roots) > 1 and source.randrange(3) == 0:
            root_vertex = source.choice(sorted(roots))
            candidates = [
                vertex for vertex in range(vertex_count)
                if root_of(vertex, parent) != root_vertex
            ]
            parent_vertex = source.choice(candidates)
            edge_weight = source.randint(-1000, 1000)
            operations.append(
                f"L {root_vertex} {parent_vertex} {edge_weight}"
            )
            parent[root_vertex] = parent_vertex
            weight[root_vertex] = edge_weight
            roots.remove(root_vertex)
        else:
            vertex = source.randrange(vertex_count)
            if source.randrange(2) == 0:
                operations.append(f"D {vertex}")
                output.append(str(distance_to_root(vertex, parent, weight)))
            else:
                operations.append(f"R {vertex}")
                output.append(str(root_of(vertex, parent)))

    return [
        f"{vertex_count} {len(operations)}",
        *operations,
    ], output


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    source = random.Random(31003)
    datasets = [build_dataset(source, 1)]
    datasets.extend(
        build_dataset(source, source.randint(2, 60))
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
