#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import heapq
import random
from pathlib import Path


def shortest_answers(
    vertex_count: int,
    edges: list[tuple[int, int, int]],
    queries: list[tuple[int, int]],
) -> list[int]:
    graph: list[list[tuple[int, int]]] = [[] for _ in range(vertex_count)]
    for source, target, weight in edges:
        graph[source].append((target, weight))

    by_source: dict[int, list[tuple[int, int]]] = {}
    for index, (source, target) in enumerate(queries):
        by_source.setdefault(source, []).append((index, target))
    answers = [-1] * len(queries)
    infinity = 10**30
    for source, requested in by_source.items():
        distances = [infinity] * vertex_count
        distances[source] = 0
        queue = [(0, source)]
        while queue:
            distance, vertex = heapq.heappop(queue)
            if distance != distances[vertex]:
                continue
            for target, weight in graph[vertex]:
                candidate = distance + weight
                if candidate < distances[target]:
                    distances[target] = candidate
                    heapq.heappush(queue, (candidate, target))
        for index, target in requested:
            if distances[target] != infinity:
                answers[index] = distances[target]
    return answers


def write_case(
    output_directory: Path,
    index: int,
    vertex_count: int,
    edges: list[tuple[int, int, int]],
    queries: list[tuple[int, int]],
) -> None:
    input_lines = [f"{vertex_count} {len(edges)} {len(queries)}"]
    input_lines.extend(f"{source} {target} {weight}" for source, target, weight in edges)
    input_lines.extend(f"{source} {target}" for source, target in queries)
    answers = shortest_answers(vertex_count, edges, queries)
    (output_directory / f"case_{index:03d}.in").write_text(
        "\n".join(input_lines) + "\n", encoding="utf-8"
    )
    (output_directory / f"case_{index:03d}.out").write_text(
        "".join(f"{answer}\n" for answer in answers), encoding="utf-8"
    )


def random_partial_two_tree(
    random_engine: random.Random,
    vertex_count: int,
) -> list[tuple[int, int, int]]:
    if vertex_count <= 1:
        return []
    host_edges = [(0, 1)]
    candidates = [(0, 1)]
    for vertex in range(2, vertex_count):
        first, second = random_engine.choice(host_edges)
        host_edges.append((first, vertex))
        host_edges.append((second, vertex))
        candidates.append((first, vertex))
        candidates.append((second, vertex))

    edges: list[tuple[int, int, int]] = []
    for first, second in candidates:
        if random_engine.randrange(6) == 0:
            continue
        if random_engine.randrange(4) != 0:
            edges.append((first, second, random_engine.randrange(31)))
        if random_engine.randrange(4) != 0:
            edges.append((second, first, random_engine.randrange(31)))
        if edges and random_engine.randrange(11) == 0:
            source, target, weight = edges[-1]
            edges.append((source, target, weight + random_engine.randrange(7)))
    for _ in range(random_engine.randrange(4)):
        vertex = random_engine.randrange(vertex_count)
        edges.append((vertex, vertex, random_engine.randrange(10)))
    return edges


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    arguments = parser.parse_args()
    arguments.out_dir.mkdir(parents=True, exist_ok=True)

    write_case(arguments.out_dir, 0, 1, [], [(0, 0)])
    write_case(
        arguments.out_dir,
        1,
        7,
        [(0, 1, 3), (1, 2, 0), (2, 0, 8), (3, 4, 2), (4, 3, 5)],
        [(0, 2), (2, 1), (0, 4), (3, 4), (6, 6)],
    )
    write_case(
        arguments.out_dir,
        2,
        6,
        [
            (0, 1, 9),
            (0, 1, 2),
            (1, 2, 4),
            (2, 3, 6),
            (3, 4, 1),
            (4, 5, 7),
            (5, 0, 3),
            (2, 2, 0),
        ],
        [(first, second) for first in range(6) for second in range(6)],
    )

    random_engine = random.Random(2026071702)
    for case_index in range(3, 43):
        vertex_count = random_engine.randint(2, 70)
        edges = random_partial_two_tree(random_engine, vertex_count)
        queries = [
            (
                random_engine.randrange(vertex_count),
                random_engine.randrange(vertex_count),
            )
            for _ in range(120)
        ]
        write_case(
            arguments.out_dir,
            case_index,
            vertex_count,
            edges,
            queries,
        )

    vertex_count = 100_000
    path_edges: list[tuple[int, int, int]] = []
    for vertex in range(vertex_count - 1):
        path_edges.append((vertex, vertex + 1, vertex % 7))
        path_edges.append((vertex + 1, vertex, 1 + vertex % 5))
    write_case(
        arguments.out_dir,
        43,
        vertex_count,
        path_edges,
        [
            (0, vertex_count - 1),
            (vertex_count - 1, 0),
            (123, 98_765),
            (98_765, 123),
            (50_000, 50_000),
            (1, 99_999),
        ],
    )

    vertex_count = 50_000
    fan_edges: list[tuple[int, int, int]] = [(0, 1, 5), (1, 0, 6)]
    for vertex in range(2, vertex_count):
        fan_edges.append((0, vertex, 10 + vertex % 13))
        fan_edges.append((vertex, 1, 20 + vertex % 17))
        if vertex % 3:
            fan_edges.append((1, vertex, 30 + vertex % 19))
    write_case(
        arguments.out_dir,
        44,
        vertex_count,
        fan_edges,
        [(2, 3), (3, 2), (0, 49_999), (49_999, 1), (1, 0)],
    )


if __name__ == "__main__":
    main()
