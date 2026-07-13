#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from collections import deque
from pathlib import Path


Edge = tuple[int, int]
Query = tuple[int, int, int, int]


def reachable(
    n: int,
    edges: list[Edge],
    first: int,
    second: int,
    banned_edge: int = -1,
    banned_vertex: int = -1,
) -> bool:
    if first == banned_vertex or second == banned_vertex:
        return False
    graph = [[] for _ in range(n)]
    for edge_id, (u, v) in enumerate(edges):
        if edge_id == banned_edge or u == banned_vertex or v == banned_vertex:
            continue
        graph[u].append(v)
        graph[v].append(u)
    seen = [False] * n
    seen[first] = True
    queue = deque([first])
    while queue:
        vertex = queue.popleft()
        for to in graph[vertex]:
            if not seen[to]:
                seen[to] = True
                queue.append(to)
    return seen[second]


def exhaustive_queries(n: int, edges: list[Edge]) -> list[Query]:
    queries: list[Query] = []
    for edge_id in range(len(edges)):
        for first in range(n):
            for second in range(n):
                queries.append((1, first, second, edge_id))
    for removed in range(n):
        for first in range(n):
            for second in range(n):
                queries.append((2, first, second, removed))
    return queries


def answer(n: int, edges: list[Edge], query: Query) -> bool:
    query_type, first, second, removed = query
    if query_type == 1:
        return reachable(n, edges, first, second, banned_edge=removed)
    return reachable(n, edges, first, second, banned_vertex=removed)


def write_case(
    output_directory: Path,
    index: int,
    n: int,
    edges: list[Edge],
    queries: list[Query],
    expected: list[bool] | None = None,
) -> None:
    name = f"case_{index:02d}"
    input_lines = [f"{n} {len(edges)} {len(queries)}"]
    input_lines.extend(f"{u} {v}" for u, v in edges)
    input_lines.extend(" ".join(map(str, query)) for query in queries)
    (output_directory / f"{name}.in").write_text(
        "\n".join(input_lines) + "\n",
        encoding="utf-8",
    )
    answers = expected if expected is not None else [answer(n, edges, query) for query in queries]
    (output_directory / f"{name}.out").write_text(
        "".join("1\n" if value else "0\n" for value in answers),
        encoding="utf-8",
    )


def random_connected_multigraph(rng: random.Random, n: int) -> list[Edge]:
    edges: list[Edge] = []
    for vertex in range(1, n):
        edges.append((vertex, rng.randrange(vertex)))
    for _ in range(rng.randrange(n, n * 3 + 1)):
        first = rng.randrange(n)
        second = rng.randrange(n)
        edges.append((first, second))
        if rng.randrange(5) == 0:
            edges.append((first, second))
    return edges


def maximum_case() -> tuple[int, list[Edge], list[Query], list[bool]]:
    n = 100000
    edge_count = 500000
    query_count = 100000
    edges: list[Edge] = [(vertex, (vertex + 1) % n) for vertex in range(n)]
    while len(edges) < edge_count:
        index = len(edges)
        vertex = index % n
        if index % 11 == 0:
            edges.append((vertex, vertex))
        elif index % 7 == 0:
            edges.append((0, 1))
        else:
            edges.append((vertex, (vertex * 1009 + index * 9176 + 12345) % n))

    queries: list[Query] = []
    expected: list[bool] = []
    for index in range(query_count):
        if index % 2 == 0:
            first = (index * 1009 + 17) % n
            second = (index * 9176 + 29) % n
            edge_id = (index * 65537 + 31) % edge_count
            queries.append((1, first, second, edge_id))
            expected.append(True)
        else:
            removed = (index * 31337 + 7) % n
            if index % 10 == 1:
                first = removed
            else:
                first = (removed + 1 + index) % n
            second = (removed + 2 + index * 3) % n
            queries.append((2, first, second, removed))
            expected.append(first != removed and second != removed)
    return n, edges, queries, expected


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    arguments = parser.parse_args()
    output_directory = Path(arguments.out_dir)
    output_directory.mkdir(parents=True, exist_ok=True)

    cases: list[tuple[int, list[Edge]]] = [
        (1, []),
        (1, [(0, 0), (0, 0)]),
        (6, [(0, 1), (1, 2), (2, 3), (3, 4), (4, 5)]),
        (7, [(i, (i + 1) % 7) for i in range(7)]),
        (6, [(0, 1), (1, 2), (2, 0), (0, 3), (3, 4), (4, 0), (0, 5)]),
        (4, [(0, 1), (0, 1), (1, 1), (1, 2), (2, 3), (3, 0), (3, 3)]),
        (8, [(0, 1), (1, 2), (2, 0), (1, 3), (3, 4), (4, 1), (3, 5), (5, 6), (6, 7)]),
    ]
    rng = random.Random(20260714)
    for n in range(2, 9):
        for _ in range(3):
            cases.append((n, random_connected_multigraph(rng, n)))

    for index, (n, edges) in enumerate(cases):
        write_case(output_directory, index, n, edges, exhaustive_queries(n, edges))

    n, edges, queries, expected = maximum_case()
    write_case(output_directory, len(cases), n, edges, queries, expected)


if __name__ == "__main__":
    main()
