#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import heapq
import random
from pathlib import Path


def shortest_path(
    vertex_count: int,
    edges: list[tuple[int, int, int]],
    source: int,
    target: int,
) -> int:
    graph: list[list[tuple[int, int]]] = [[] for _ in range(vertex_count)]
    for first, second, cost in edges:
        graph[first].append((second, cost))
    infinity = 10**30
    distance = [infinity] * vertex_count
    distance[source] = 0
    queue = [(0, source)]
    while queue:
        current_distance, vertex = heapq.heappop(queue)
        if current_distance != distance[vertex]:
            continue
        for to, cost in graph[vertex]:
            candidate = current_distance + cost
            if candidate >= distance[to]:
                continue
            distance[to] = candidate
            heapq.heappush(queue, (candidate, to))
    return -1 if distance[target] == infinity else distance[target]


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(20260729102)

    cases: list[tuple[int, list[tuple[int, int, int]], int, int]] = [
        (1, [], 0, 0),
        (4, [(0, 1, 5), (1, 2, 0), (2, 3, 7)], 0, 3),
        (4, [(0, 1, 2), (0, 1, 9), (2, 3, 1)], 0, 3),
        (5, [(0, 1, 4), (1, 0, 3), (3, 4, 2)], 4, 3),
    ]
    for _ in range(52):
        vertex_count = rng.randrange(1, 10)
        edges: list[tuple[int, int, int]] = []
        for first in range(vertex_count):
            for second in range(vertex_count):
                if rng.randrange(5) == 0:
                    edges.append((first, second, rng.randrange(16)))
                    if rng.randrange(9) == 0:
                        edges.append((first, second, rng.randrange(16)))
        cases.append(
            (
                vertex_count,
                edges,
                rng.randrange(vertex_count),
                rng.randrange(vertex_count),
            )
        )

    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for vertex_count, edges, source, target in cases:
        input_lines.append(
            f"{vertex_count} {len(edges)} {source} {target}"
        )
        input_lines.extend(
            f"{first} {second} {cost}" for first, second, cost in edges
        )
        answer = shortest_path(
            vertex_count, edges, source, target
        )
        output_lines.append(" ".join([str(answer)] * 5))

    (out_dir / "case_00.in").write_text(
        "\n".join(input_lines) + "\n", encoding="utf-8"
    )
    (out_dir / "case_00.out").write_text(
        "\n".join(output_lines) + "\n", encoding="utf-8"
    )


if __name__ == "__main__":
    main()
