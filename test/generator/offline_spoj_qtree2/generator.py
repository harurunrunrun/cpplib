#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


Edge = tuple[int, int, int]
Query = tuple[str, int, int, int]
TreeCase = tuple[int, list[Edge], list[Query]]


def graph_from_edges(size: int, edges: list[Edge]):
    graph: list[list[tuple[int, int]]] = [[] for _ in range(size)]
    for left, right, weight in edges:
        graph[left].append((right, weight))
        graph[right].append((left, weight))
    return graph


def path_and_distance(graph, source: int, target: int):
    parent = [-1] * len(graph)
    parent_weight = [0] * len(graph)
    parent[source] = source
    stack = [source]
    while stack:
        vertex = stack.pop()
        if vertex == target:
            break
        for next_vertex, weight in graph[vertex]:
            if parent[next_vertex] == -1:
                parent[next_vertex] = vertex
                parent_weight[next_vertex] = weight
                stack.append(next_vertex)

    path = []
    distance = 0
    vertex = target
    while vertex != source:
        path.append(vertex)
        distance += parent_weight[vertex]
        vertex = parent[vertex]
    path.append(source)
    path.reverse()
    return path, distance


def answer_queries(size: int, edges: list[Edge], queries: list[Query]):
    graph = graph_from_edges(size, edges)
    answers: list[str] = []
    for command, left, right, k in queries:
        path, distance = path_and_distance(graph, left, right)
        if command == "DIST":
            answers.append(str(distance))
        else:
            answers.append(str(path[k - 1] + 1))
    return answers


def random_tree(size: int, rng: random.Random) -> list[Edge]:
    edges = [
        (rng.randrange(vertex), vertex, rng.randrange(1, 100_001))
        for vertex in range(1, size)
    ]
    rng.shuffle(edges)
    return edges


def chain_tree(size: int) -> list[Edge]:
    return [
        (vertex - 1, vertex, 1 + (vertex * 99_991) % 100_000)
        for vertex in range(1, size)
    ]


def random_queries(
    size: int,
    edges: list[Edge],
    count: int,
    rng: random.Random,
) -> list[Query]:
    graph = graph_from_edges(size, edges)
    queries: list[Query] = []
    for index in range(count):
        left = rng.randrange(size)
        right = rng.randrange(size)
        if index % 2 == 0:
            queries.append(("DIST", left, right, 0))
        else:
            path, _ = path_and_distance(graph, left, right)
            queries.append(("KTH", left, right, rng.randrange(1, len(path) + 1)))
    return queries


def write_case(output_directory: Path, name: str, trees: list[TreeCase]):
    input_lines = [str(len(trees))]
    output_lines: list[str] = []
    for size, edges, queries in trees:
        input_lines.append(str(size))
        input_lines.extend(
            f"{left + 1} {right + 1} {weight}"
            for left, right, weight in edges
        )
        for command, left, right, k in queries:
            if command == "DIST":
                input_lines.append(f"DIST {left + 1} {right + 1}")
            else:
                input_lines.append(f"KTH {left + 1} {right + 1} {k}")
        input_lines.append("DONE")
        output_lines.extend(answer_queries(size, edges, queries))
        output_lines.append("")

    (output_directory / f"{name}.in").write_text(
        "\n".join(input_lines) + "\n",
        encoding="utf-8",
    )
    (output_directory / f"{name}.out").write_text(
        "\n".join(output_lines) + "\n",
        encoding="utf-8",
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    output_directory = Path(parser.parse_args().out_dir)
    output_directory.mkdir(parents=True, exist_ok=True)
    rng = random.Random(2026071402)

    single_queries: list[Query] = [
        ("DIST", 0, 0, 0),
        ("KTH", 0, 0, 1),
    ]

    chain_edges = chain_tree(9)
    chain_queries: list[Query] = [
        ("DIST", 0, 8, 0),
        ("DIST", 8, 0, 0),
        ("DIST", 4, 4, 0),
    ]
    chain_queries.extend(("KTH", 0, 8, k) for k in range(1, 10))
    chain_queries.extend(("KTH", 8, 0, k) for k in range(1, 10))

    star_edges = [
        (0, vertex, 100_000 if vertex % 2 else vertex)
        for vertex in range(1, 13)
    ]
    star_queries = random_queries(13, star_edges, 80, rng)
    star_queries.extend(
        [
            ("DIST", 1, 2, 0),
            ("KTH", 1, 2, 1),
            ("KTH", 1, 2, 2),
            ("KTH", 1, 2, 3),
        ]
    )
    write_case(
        output_directory,
        "case_00_boundaries",
        [
            (1, [], single_queries),
            (9, chain_edges, chain_queries),
            (13, star_edges, star_queries),
        ],
    )

    small_trees: list[TreeCase] = []
    for _ in range(24):
        size = rng.randrange(2, 81)
        edges = random_tree(size, rng)
        small_trees.append((size, edges, random_queries(size, edges, 120, rng)))
    write_case(output_directory, "case_01_random_small", small_trees)

    large_edges = random_tree(1000, rng)
    write_case(
        output_directory,
        "case_02_random_large",
        [(1000, large_edges, random_queries(1000, large_edges, 3000, rng))],
    )

    maximum_edges = chain_tree(10_000)
    maximum_queries = random_queries(10_000, maximum_edges, 256, rng)
    maximum_queries.extend(
        [
            ("DIST", 0, 9999, 0),
            ("KTH", 0, 9999, 1),
            ("KTH", 0, 9999, 10_000),
            ("KTH", 9999, 0, 10_000),
        ]
    )
    write_case(
        output_directory,
        "case_03_maximum_chain",
        [(10_000, maximum_edges, maximum_queries)],
    )


if __name__ == "__main__":
    main()
