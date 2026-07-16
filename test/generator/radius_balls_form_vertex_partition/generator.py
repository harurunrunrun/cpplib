#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import collections
import random
from pathlib import Path

Case = tuple[int, list[tuple[int, int]], list[tuple[int, int]]]


def answer(case: Case) -> bool:
    vertex_count, edges, balls = case
    graph = [[] for _ in range(vertex_count)]
    for left, right in edges:
        graph[left].append(right)
        graph[right].append(left)
    coverage = [0] * vertex_count
    for center, radius in balls:
        distance = [-1] * vertex_count
        distance[center] = 0
        queue = collections.deque([center])
        while queue:
            vertex = queue.popleft()
            coverage[vertex] += 1
            if distance[vertex] == radius:
                continue
            for next_vertex in graph[vertex]:
                if distance[next_vertex] == -1:
                    distance[next_vertex] = distance[vertex] + 1
                    queue.append(next_vertex)
    return all(count == 1 for count in coverage)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[Case] = [
        (0, [], []),
        (1, [], [(0, 0)]),
        (2, [(0, 1)], [(0, 0), (1, 0)]),
        (3, [(0, 1), (1, 2)], [(0, 1), (2, 1)]),
        (4, [(0, 1), (2, 3)], [(0, 1)]),
    ]
    source = random.Random(31004)
    for _ in range(180):
        vertex_count = source.randint(1, 14)
        possible = [
            (left, right)
            for left in range(vertex_count)
            for right in range(left + 1, vertex_count)
        ]
        source.shuffle(possible)
        edges = possible[:source.randint(0, len(possible))]
        balls = [
            (
                source.randrange(vertex_count),
                source.randint(0, vertex_count),
            )
            for _ in range(source.randint(0, min(7, vertex_count + 2)))
        ]
        cases.append((vertex_count, edges, balls))

    input_lines = [str(len(cases))]
    output_lines = []
    for vertex_count, edges, radius_balls in cases:
        input_lines.append(
            f"{vertex_count} {len(edges)} {len(radius_balls)}"
        )
        input_lines.extend(f"{left} {right}" for left, right in edges)
        input_lines.extend(
            f"{center} {radius}" for center, radius in radius_balls
        )
        output_lines.append(
            "YES" if answer(
                (vertex_count, edges, radius_balls)
            ) else "NO"
        )

    (out_dir / "case_00.in").write_text(
        "\n".join(input_lines) + "\n", encoding="ascii"
    )
    (out_dir / "case_00.out").write_text(
        "\n".join(output_lines) + "\n", encoding="ascii"
    )


if __name__ == "__main__":
    main()
