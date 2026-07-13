#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import collections
import random
from pathlib import Path


Split = tuple[int, int]
Edge = tuple[int, int]
MASK64 = (1 << 64) - 1


def adjacency(vertex_count: int, edges: list[Edge]) -> list[list[tuple[int, int]]]:
    graph: list[list[tuple[int, int]]] = [[] for _ in range(vertex_count)]
    for index, (left, right) in enumerate(edges):
        graph[left].append((right, index))
        graph[right].append((left, index))
    return graph


def farthest(
    graph: list[list[tuple[int, int]]],
    start: int,
    removed_edge: int,
) -> tuple[int, int]:
    distance = [-1] * len(graph)
    distance[start] = 0
    queue = collections.deque([start])
    best = start
    while queue:
        vertex = queue.popleft()
        if distance[best] < distance[vertex]:
            best = vertex
        for to, edge in graph[vertex]:
            if edge == removed_edge or distance[to] != -1:
                continue
            distance[to] = distance[vertex] + 1
            queue.append(to)
    return best, distance[best]


def component_diameter(
    graph: list[list[tuple[int, int]]],
    start: int,
    removed_edge: int,
) -> int:
    endpoint, _ = farthest(graph, start, removed_edge)
    return farthest(graph, endpoint, removed_edge)[1]


def brute_splits(vertex_count: int, edges: list[Edge]) -> list[Split]:
    graph = adjacency(vertex_count, edges)
    return [
        (
            component_diameter(graph, left, edge),
            component_diameter(graph, right, edge),
        )
        for edge, (left, right) in enumerate(edges)
    ]


def best_result(edges: list[Edge], split: list[Split]) -> tuple[int, int, int, int, int, int]:
    if not edges:
        return 0, -1, -1, -1, 0, 0
    best_edge = max(range(len(edges)), key=lambda edge: split[edge][0] * split[edge][1])
    first, second = split[best_edge]
    left, right = edges[best_edge]
    return first * second, best_edge, left, right, first, second


def output_line(edges: list[Edge], split: list[Split], detailed: bool) -> str:
    values = list(best_result(edges, split))
    if detailed:
        for first, second in split:
            values.extend((first, second))
    else:
        first_hash = sum(
            (edge + 1) * (first + 1)
            for edge, (first, _) in enumerate(split)
        ) & MASK64
        second_hash = sum(
            (edge + 1) * (second + 1)
            for edge, (_, second) in enumerate(split)
        ) & MASK64
        values.extend((first_hash, second_hash))
    return " ".join(map(str, values))


def write_cases(
    path: Path,
    cases: list[tuple[int, list[Edge], list[Split], bool]],
) -> None:
    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for vertex_count, edges, split, detailed in cases:
        input_lines.append(f"{vertex_count} {int(detailed)}")
        input_lines.extend(f"{left} {right}" for left, right in edges)
        output_lines.append(output_line(edges, split, detailed))
    (path.with_suffix(".in")).write_text("\n".join(input_lines) + "\n", encoding="utf-8")
    (path.with_suffix(".out")).write_text("\n".join(output_lines) + "\n", encoding="utf-8")


def orient_edges(edges: list[Edge]) -> list[Edge]:
    return [edge if index % 2 == 0 else (edge[1], edge[0]) for index, edge in enumerate(edges)]


def random_tree(rng: random.Random, vertex_count: int) -> list[Edge]:
    edges = [(vertex, rng.randrange(vertex)) for vertex in range(1, vertex_count)]
    rng.shuffle(edges)
    return [
        edge if rng.randrange(2) == 0 else (edge[1], edge[0])
        for edge in edges
    ]


def small_cases() -> list[tuple[int, list[Edge], list[Split], bool]]:
    rng = random.Random(20260714)
    raw: list[tuple[int, list[Edge]]] = [
        (1, []),
        (2, [(1, 0)]),
        (4, [(0, 1), (2, 1), (2, 3)]),
        (8, orient_edges([(0, vertex) for vertex in range(1, 8)])),
        (
            13,
            orient_edges(
                [(vertex, vertex + 1) for vertex in range(7)]
                + [(7, leaf) for leaf in range(8, 13)]
            ),
        ),
        (15, orient_edges([((vertex - 1) // 2, vertex) for vertex in range(1, 15)])),
    ]
    for _ in range(150):
        vertex_count = rng.randrange(1, 43)
        raw.append((vertex_count, random_tree(rng, vertex_count)))
    return [
        (vertex_count, edges, brute_splits(vertex_count, edges), True)
        for vertex_count, edges in raw
    ]


def chain_case(vertex_count: int) -> tuple[int, list[Edge], list[Split], bool]:
    edges: list[Edge] = []
    split: list[Split] = []
    for edge in range(vertex_count - 1):
        endpoints = (edge, edge + 1)
        diameters = (edge, vertex_count - edge - 2)
        if edge % 2 != 0:
            endpoints = (endpoints[1], endpoints[0])
            diameters = (diameters[1], diameters[0])
        edges.append(endpoints)
        split.append(diameters)
    return vertex_count, edges, split, False


def star_case(vertex_count: int) -> tuple[int, list[Edge], list[Split], bool]:
    edges: list[Edge] = []
    split: list[Split] = []
    remaining_diameter = 2 if vertex_count >= 4 else vertex_count - 2
    for edge, leaf in enumerate(range(1, vertex_count)):
        if edge % 2 == 0:
            edges.append((0, leaf))
            split.append((remaining_diameter, 0))
        else:
            edges.append((leaf, 0))
            split.append((0, remaining_diameter))
    return vertex_count, edges, split, False


def broom_case(
    vertex_count: int,
    handle_end: int,
) -> tuple[int, list[Edge], list[Split], bool]:
    edges: list[Edge] = []
    split: list[Split] = []
    raw_edges = [(vertex, vertex + 1) for vertex in range(handle_end)]
    raw_edges += [(handle_end, leaf) for leaf in range(handle_end + 1, vertex_count)]
    for edge, endpoints in enumerate(raw_edges):
        if edge < handle_end:
            diameters = (edge, max(handle_end - edge, 2))
        else:
            diameters = (handle_end + 1, 0)
        if edge % 2 != 0:
            endpoints = (endpoints[1], endpoints[0])
            diameters = (diameters[1], diameters[0])
        edges.append(endpoints)
        split.append(diameters)
    return vertex_count, edges, split, False


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    write_cases(out_dir / "case_00", small_cases())
    write_cases(out_dir / "case_01", [chain_case(100_000)])
    write_cases(out_dir / "case_02", [star_case(100_000)])
    write_cases(out_dir / "case_03", [broom_case(100_000, 60_000)])


if __name__ == "__main__":
    main()
