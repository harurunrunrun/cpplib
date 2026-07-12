#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def adjacency(n: int, edges: list[tuple[int, int, int]]) -> list[list[tuple[int, int]]]:
    graph: list[list[tuple[int, int]]] = [[] for _ in range(n)]
    for u, v, weight in edges:
        graph[u].append((v, weight))
        graph[v].append((u, weight))
    return graph


def distances(graph: list[list[tuple[int, int]]], root: int) -> tuple[list[int], list[int]]:
    unweighted = [-1] * len(graph)
    weighted = [0] * len(graph)
    unweighted[root] = 0
    stack = [root]
    while stack:
        vertex = stack.pop()
        for to, weight in graph[vertex]:
            if unweighted[to] != -1:
                continue
            unweighted[to] = unweighted[vertex] + 1
            weighted[to] = weighted[vertex] + weight
            stack.append(to)
    return unweighted, weighted


def centroids(graph: list[list[tuple[int, int]]]) -> list[int]:
    n = len(graph)
    result: list[int] = []
    for removed in range(n):
        largest = 0
        seen = {removed}
        for start, _ in graph[removed]:
            if start in seen:
                continue
            seen.add(start)
            stack = [start]
            size = 0
            while stack:
                vertex = stack.pop()
                size += 1
                for to, _ in graph[vertex]:
                    if to not in seen:
                        seen.add(to)
                        stack.append(to)
            largest = max(largest, size)
        if largest * 2 <= n:
            result.append(removed)
    return result


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(20260713)
    cases: list[tuple[int, list[tuple[int, int, int]]]] = [
        (1, []),
        (2, [(0, 1, 0)]),
        (6, [(0, 1, 4), (1, 2, 7), (1, 3, 2), (3, 4, 5), (3, 5, 1)]),
        (8, [(i, i + 1, i * i) for i in range(7)]),
        (7, [(0, i, 1_000_000_000_000 + i) for i in range(1, 7)]),
    ]
    for _ in range(90):
        n = rng.randrange(1, 81)
        edges = [(vertex, rng.randrange(vertex), rng.randrange(1_000_000_000_001)) for vertex in range(1, n)]
        rng.shuffle(edges)
        cases.append((n, edges))

    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for n, edges in cases:
        input_lines.append(str(n))
        input_lines.extend(f"{u} {v} {weight}" for u, v, weight in edges)
        graph = adjacency(n, edges)
        all_distances = [distances(graph, root) for root in range(n)]
        diameter = max(max(unweighted) for unweighted, _ in all_distances)
        weighted_diameter = max(max(weighted) for _, weighted in all_distances)
        centers = centroids(graph)
        output_lines.append(" ".join(map(str, [diameter, weighted_diameter, len(centers), *centers])))

    (out_dir / "case_00.in").write_text("\n".join(input_lines) + "\n", encoding="utf-8")
    (out_dir / "case_00.out").write_text("\n".join(output_lines) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
