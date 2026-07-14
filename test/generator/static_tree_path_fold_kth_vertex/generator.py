#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from collections import deque
from pathlib import Path


def path_between(
    graph: list[list[int]], source: int, target: int
) -> list[int]:
    parent = [-1] * len(graph)
    parent[source] = source
    queue = deque([source])
    while queue:
        vertex = queue.popleft()
        if vertex == target:
            break
        for neighbor in graph[vertex]:
            if parent[neighbor] != -1:
                continue
            parent[neighbor] = vertex
            queue.append(neighbor)
    path = []
    vertex = target
    while vertex != source:
        path.append(vertex)
        vertex = parent[vertex]
    path.append(source)
    path.reverse()
    return path


def write_case(
    out_dir: Path,
    index: int,
    vertex_count: int,
    root: int,
    edges: list[tuple[int, int]],
    endpoints: list[tuple[int, int]],
) -> None:
    graph = [[] for _ in range(vertex_count)]
    for left, right in edges:
        graph[left].append(right)
        graph[right].append(left)
    queries = []
    answers = []
    for left, right in endpoints:
        path = path_between(graph, left, right)
        indices = {0, len(path) - 1, len(path) // 2}
        for position in sorted(indices):
            queries.append((left, right, position))
            answers.append(path[position])

    name = f"case_{index:03d}"
    input_lines = [
        f"{vertex_count} {root} {len(queries)}",
        *(f"{left} {right}" for left, right in edges),
        *(f"{left} {right} {position}" for left, right, position in queries),
    ]
    (out_dir / f"{name}.in").write_text(
        "\n".join(input_lines) + "\n", encoding="ascii"
    )
    (out_dir / f"{name}.out").write_text(
        "\n".join(map(str, answers)) + "\n", encoding="ascii"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    write_case(out_dir, 0, 1, 0, [], [(0, 0)])
    write_case(
        out_dir,
        1,
        7,
        3,
        [(0, 1), (1, 2), (1, 3), (3, 4), (3, 5), (5, 6)],
        [(0, 6), (2, 4), (6, 0), (3, 3)],
    )

    rng = random.Random(27182818)
    for index in range(2, 52):
        vertex_count = rng.randint(1, 80)
        edges = [
            (vertex, rng.randrange(vertex))
            for vertex in range(1, vertex_count)
        ]
        rng.shuffle(edges)
        endpoints = [
            (rng.randrange(vertex_count), rng.randrange(vertex_count))
            for _ in range(120)
        ]
        write_case(
            out_dir,
            index,
            vertex_count,
            rng.randrange(vertex_count),
            edges,
            endpoints,
        )


if __name__ == "__main__":
    main()
