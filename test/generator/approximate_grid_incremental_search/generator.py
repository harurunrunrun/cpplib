#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import heapq
import random
from pathlib import Path


def shortest_distance(
    vertex_count: int,
    edges: dict[tuple[int, int], int],
    start: int,
    goal: int,
) -> int:
    graph: list[list[tuple[int, int]]] = [[] for _ in range(vertex_count)]
    for (source, target), cost in edges.items():
        graph[source].append((target, cost))
    infinity = 10**30
    distance = [infinity] * vertex_count
    distance[start] = 0
    queue: list[tuple[int, int]] = [(0, start)]
    while queue:
        current_distance, vertex = heapq.heappop(queue)
        if current_distance != distance[vertex]:
            continue
        for target, cost in graph[vertex]:
            candidate = current_distance + cost
            if candidate < distance[target]:
                distance[target] = candidate
                heapq.heappush(queue, (candidate, target))
    return -1 if distance[goal] == infinity else distance[goal]


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    arguments = parser.parse_args()
    output_directory = Path(arguments.out_dir)
    output_directory.mkdir(parents=True, exist_ok=True)
    random_engine = random.Random(2026072923)

    cases: list[
        tuple[
            int,
            int,
            int,
            dict[tuple[int, int], int],
            list[tuple[int, int, int, int]],
        ]
    ] = [
        (1, 0, 0, {}, []),
        (
            4,
            0,
            3,
            {(0, 1): 2, (1, 3): 2, (0, 2): 1, (2, 3): 8},
            [
                (1, 2, 3, 1),
                (0, 0, 2, 0),
                (0, 1, 3, 0),
                (1, 0, 3, 12),
                (1, 1, 3, 2),
            ],
        ),
        (
            3,
            0,
            2,
            {(0, 1): 5},
            [(1, 1, 2, 7), (0, 0, 1, 0), (1, 0, 2, 20)],
        ),
    ]
    for _ in range(35):
        vertex_count = random_engine.randrange(2, 18)
        start = random_engine.randrange(vertex_count)
        goal = random_engine.randrange(vertex_count)
        edges: dict[tuple[int, int], int] = {}
        for source in range(vertex_count):
            for target in range(vertex_count):
                if source != target and random_engine.randrange(5) == 0:
                    edges[(source, target)] = random_engine.randrange(1, 51)
        updates: list[tuple[int, int, int, int]] = []
        for _ in range(30):
            source = random_engine.randrange(vertex_count)
            target = random_engine.randrange(vertex_count - 1)
            if target >= source:
                target += 1
            if random_engine.randrange(4) == 0:
                updates.append((0, source, target, 0))
            else:
                updates.append(
                    (1, source, target, random_engine.randrange(1, 51))
                )
        cases.append((vertex_count, start, goal, edges, updates))

    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for vertex_count, start, goal, initial_edges, updates in cases:
        input_lines.append(
            f"{vertex_count} {len(initial_edges)} {start} {goal} {len(updates)}"
        )
        input_lines.extend(
            f"{source} {target} {cost}"
            for (source, target), cost in sorted(initial_edges.items())
        )
        edges = dict(initial_edges)
        answer = shortest_distance(vertex_count, edges, start, goal)
        output_lines.append(" ".join([str(answer)] * 5))
        for operation, source, target, cost in updates:
            input_lines.append(f"{operation} {source} {target} {cost}")
            if operation == 0:
                edges.pop((source, target), None)
            else:
                edges[(source, target)] = cost
            answer = shortest_distance(vertex_count, edges, start, goal)
            output_lines.append(" ".join([str(answer)] * 5))

    (output_directory / "case_00.in").write_text(
        "\n".join(input_lines) + "\n",
        encoding="utf-8",
    )
    (output_directory / "case_00.out").write_text(
        "\n".join(output_lines) + "\n",
        encoding="utf-8",
    )


if __name__ == "__main__":
    main()
