#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import heapq
import random
from pathlib import Path


def shortest(graph: list[list[tuple[int, int]]], source: int, target: int) -> int:
    infinity = 10**30
    distance = [infinity] * len(graph)
    distance[source] = 0
    queue = [(0, source)]
    while queue:
        current, vertex = heapq.heappop(queue)
        if current != distance[vertex]:
            continue
        for to, cost in graph[vertex]:
            candidate = current + cost
            if candidate < distance[to]:
                distance[to] = candidate
                heapq.heappush(queue, (candidate, to))
    return -1 if distance[target] == infinity else distance[target]


def write_case(
    output: Path,
    index: int,
    graph: list[list[tuple[int, int]]],
    source: int,
    target: int,
) -> None:
    edges = [
        (vertex, to, cost)
        for vertex, adjacent in enumerate(graph)
        for to, cost in adjacent
    ]
    lines = [f"{len(graph)} {len(edges)} {source} {target}"]
    lines.extend(f"{u} {v} {cost}" for u, v, cost in edges)
    stem = f"case_{index:02d}"
    (output / f"{stem}.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    answer = shortest(graph, source, target)
    (output / f"{stem}.out").write_text(f"{answer}\n", encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    arguments = parser.parse_args()
    output = Path(arguments.out_dir)
    output.mkdir(parents=True, exist_ok=True)
    cases: list[tuple[list[list[tuple[int, int]]], int, int]] = [
        ([[]], 0, 0),
        ([[(1, 5)], [], []], 0, 2),
        ([[(1, 20), (2, 2)], [(3, 1)], [(1, 3), (3, 9)], []], 0, 3),
    ]
    engine = random.Random(20260729)
    for n, denominator in ((8, 3), (25, 7), (60, 13), (120, 23)):
        graph: list[list[tuple[int, int]]] = [[] for _ in range(n)]
        for u in range(n):
            for v in range(n):
                if u != v and engine.randrange(denominator) == 0:
                    graph[u].append((v, engine.randrange(1, 10_001)))
        cases.append((graph, engine.randrange(n), engine.randrange(n)))
    for index, case in enumerate(cases):
        write_case(output, index, *case)


if __name__ == "__main__":
    main()
