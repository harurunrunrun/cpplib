#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import collections
import random
from pathlib import Path


def shortest_path(graph: list[list[int]], source: int, target: int) -> int:
    distance = [-1] * len(graph)
    distance[source] = 0
    queue = collections.deque([source])
    while queue:
        vertex = queue.popleft()
        for to in graph[vertex]:
            if distance[to] != -1:
                continue
            distance[to] = distance[vertex] + 1
            queue.append(to)
    return distance[target]


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(2026072901)

    cases: list[tuple[int, list[tuple[int, int]], int, int]] = [
        (1, [], 0, 0),
        (4, [(0, 1), (0, 2), (2, 3)], 0, 3),
        (4, [(0, 1), (1, 2)], 0, 3),
    ]
    for _ in range(45):
        vertex_count = rng.randrange(2, 11)
        edges: list[tuple[int, int]] = []
        for first in range(vertex_count):
            for second in range(first + 1, vertex_count):
                if rng.randrange(4) == 0:
                    edges.append((first, second))
        source = rng.randrange(vertex_count)
        target = rng.randrange(vertex_count)
        cases.append((vertex_count, edges, source, target))

    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for vertex_count, edges, source, target in cases:
        input_lines.append(
            f"{vertex_count} {len(edges)} {source} {target}"
        )
        input_lines.extend(f"{first} {second}" for first, second in edges)
        graph = [[] for _ in range(vertex_count)]
        for first, second in edges:
            graph[first].append(second)
        answer = shortest_path(graph, source, target)
        output_lines.append(" ".join([str(answer)] * 5))

    (out_dir / "case_00.in").write_text(
        "\n".join(input_lines) + "\n", encoding="utf-8"
    )
    (out_dir / "case_00.out").write_text(
        "\n".join(output_lines) + "\n", encoding="utf-8"
    )


if __name__ == "__main__":
    main()
