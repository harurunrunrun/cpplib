#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import heapq
import random
from pathlib import Path


def shortest_paths(matrix: list[list[int]], source: int) -> list[int]:
    vertex_count = len(matrix)
    distance = [10**30] * vertex_count
    used = [False] * vertex_count
    distance[source] = 0
    for _ in range(vertex_count):
        vertex = min(
            (index for index in range(vertex_count) if not used[index]),
            key=distance.__getitem__,
        )
        used[vertex] = True
        for target, cost in enumerate(matrix[vertex]):
            distance[target] = min(
                distance[target], distance[vertex] + cost
            )
    return distance


def write_case(
    out_dir: Path,
    index: int,
    matrix: list[list[int]],
    queries: list[tuple[int, int]],
) -> None:
    cache: dict[int, list[int]] = {}
    answer = 0
    for source, target in queries:
        if source not in cache:
            cache[source] = shortest_paths(matrix, source)
        answer += cache[source][target]

    name = f"case_{index:03d}"
    input_lines = [
        f"M {len(matrix)} {len(queries)}",
        *(" ".join(map(str, row)) for row in matrix),
        *(f"{source} {target}" for source, target in queries),
    ]
    (out_dir / f"{name}.in").write_text(
        "\n".join(input_lines) + "\n", encoding="ascii"
    )
    (out_dir / f"{name}.out").write_text(f"{answer}\n", encoding="ascii")


def sparse_distances(
    vertex_count: int,
    graph: list[list[tuple[int, int]]],
    source: int,
) -> list[int]:
    infinity = 10**30
    distance = [infinity] * vertex_count
    distance[source] = 0
    queue = [(0, source)]
    while queue:
        current, vertex = heapq.heappop(queue)
        if current != distance[vertex]:
            continue
        for target, cost in graph[vertex]:
            candidate = current + cost
            if candidate < distance[target]:
                distance[target] = candidate
                heapq.heappush(queue, (candidate, target))
    return distance


def write_sparse_case(
    out_dir: Path,
    index: int,
    mode: str,
    vertex_count: int,
    edges: list[tuple[int, int, int]],
    queries: list[tuple[int, int]],
) -> None:
    infinity = 10**30
    if mode == "N":
        graph: list[list[tuple[int, int]]] = [
            [] for _ in range(vertex_count)
        ]
        for source, target, cost in edges:
            graph[source].append((target, cost))
        cache = {
            source: sparse_distances(vertex_count, graph, source)
            for source in {source for source, _ in queries}
        }
    else:
        distance = [
            [infinity] * vertex_count for _ in range(vertex_count)
        ]
        for vertex in range(vertex_count):
            distance[vertex][vertex] = 0
        for source, target, cost in edges:
            distance[source][target] = min(distance[source][target], cost)
        for middle in range(vertex_count):
            for source in range(vertex_count):
                if distance[source][middle] == infinity:
                    continue
                for target in range(vertex_count):
                    if distance[middle][target] == infinity:
                        continue
                    distance[source][target] = min(
                        distance[source][target],
                        distance[source][middle] + distance[middle][target],
                    )
        assert all(distance[vertex][vertex] >= 0 for vertex in range(vertex_count))
        cache = {
            source: distance[source]
            for source in {source for source, _ in queries}
        }
    answer = sum(cache[source][target] for source, target in queries)
    assert answer < infinity

    name = f"case_{index:03d}"
    input_lines = [
        f"{mode} {vertex_count} {len(edges)} {len(queries)}",
        *(f"{source} {target} {cost}" for source, target, cost in edges),
        *(f"{source} {target}" for source, target in queries),
    ]
    (out_dir / f"{name}.in").write_text(
        "\n".join(input_lines) + "\n", encoding="ascii"
    )
    (out_dir / f"{name}.out").write_text(f"{answer}\n", encoding="ascii")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    write_case(out_dir, 0, [[0]], [(0, 0), (0, 0)])
    write_case(
        out_dir,
        1,
        [[0, 1, 1], [1, 0, 1], [1, 9, 0]],
        [(1, 0), (2, 1)],
    )

    rng = random.Random(11451419)
    for index in range(2, 42):
        vertex_count = rng.randint(1, 12)
        matrix = [
            [
                0 if source == target else rng.randint(1, 200)
                for target in range(vertex_count)
            ]
            for source in range(vertex_count)
        ]
        queries = [
            (rng.randrange(vertex_count), rng.randrange(vertex_count))
            for _ in range(rng.randint(1, 80))
        ]
        write_case(out_dir, index, matrix, queries)

    index = 42
    for _ in range(16):
        vertex_count = rng.randint(2, 18)
        edges = [
            (vertex, (vertex + 1) % vertex_count, rng.randint(0, 20))
            for vertex in range(vertex_count)
        ]
        edges += [
            ((vertex + 1) % vertex_count, vertex, rng.randint(0, 20))
            for vertex in range(vertex_count)
        ]
        for _ in range(rng.randint(vertex_count, 4 * vertex_count)):
            edges.append((
                rng.randrange(vertex_count),
                rng.randrange(vertex_count),
                rng.randint(0, 100),
            ))
        queries = [
            (rng.randrange(vertex_count), rng.randrange(vertex_count))
            for _ in range(rng.randint(1, 100))
        ]
        write_sparse_case(
            out_dir, index, "N", vertex_count, edges, queries
        )
        index += 1

    for _ in range(16):
        vertex_count = rng.randint(2, 14)
        potential = [
            rng.randint(-100, 100) for _ in range(vertex_count)
        ]
        reduced_edges = [
            (vertex, (vertex + 1) % vertex_count, rng.randint(0, 30))
            for vertex in range(vertex_count)
        ]
        reduced_edges += [
            ((vertex + 1) % vertex_count, vertex, rng.randint(0, 30))
            for vertex in range(vertex_count)
        ]
        for _ in range(rng.randint(vertex_count, 4 * vertex_count)):
            reduced_edges.append((
                rng.randrange(vertex_count),
                rng.randrange(vertex_count),
                rng.randint(0, 100),
            ))
        edges = [
            (source, target, cost - potential[source] + potential[target])
            for source, target, cost in reduced_edges
        ]
        queries = [
            (rng.randrange(vertex_count), rng.randrange(vertex_count))
            for _ in range(rng.randint(1, 100))
        ]
        write_sparse_case(
            out_dir, index, "J", vertex_count, edges, queries
        )
        index += 1

    vertex_count = 10_000
    edges = [
        (vertex, vertex + 1, 1) for vertex in range(vertex_count - 1)
    ] + [
        (vertex + 1, vertex, 2) for vertex in range(vertex_count - 1)
    ]
    queries = [(0, vertex_count - 1), (0, 5000), (9999, 0)]
    write_sparse_case(
        out_dir, index, "N", vertex_count, edges, queries
    )


if __name__ == "__main__":
    main()
