#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import heapq
import random
from pathlib import Path


def shortest(n: int, edges: list[tuple[int, int, int]], source: int, target: int) -> int:
    graph: list[list[tuple[int, int]]] = [[] for _ in range(n)]
    for u, v, cost in edges:
        graph[u].append((v, cost))
    infinity = 10**30
    distance = [infinity] * n
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
    output_directory: Path,
    index: int,
    n: int,
    edges: list[tuple[int, int, int]],
    source: int,
    target: int,
) -> None:
    distance = shortest(n, edges, source, target)
    stem = f"case_{index:02d}"
    lines = [f"{n} {len(edges)} {source} {target}"]
    lines.extend(f"{u} {v} {cost}" for u, v, cost in edges)
    (output_directory / f"{stem}.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    reachable = int(distance != -1)
    (output_directory / f"{stem}.out").write_text(
        f"{distance} {distance} {reachable} {distance}\n", encoding="utf-8"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    arguments = parser.parse_args()
    output_directory = Path(arguments.out_dir)
    output_directory.mkdir(parents=True, exist_ok=True)

    cases: list[tuple[int, list[tuple[int, int, int]], int, int]] = [
        (1, [], 0, 0),
        (4, [(0, 1, 5), (1, 2, 7)], 0, 3),
        (5, [(0, 1, 100), (0, 2, 3), (2, 1, 4), (1, 4, 2), (2, 4, 20)], 0, 4),
    ]
    random_engine = random.Random(20260713)
    for n, modulus in [(6, 3), (15, 5), (40, 8), (80, 12)]:
        edges: list[tuple[int, int, int]] = []
        for u in range(n):
            for v in range(n):
                if u != v and random_engine.randrange(modulus) == 0:
                    edges.append((u, v, random_engine.randrange(1, 10_000)))
        cases.append((n, edges, random_engine.randrange(n), random_engine.randrange(n)))

    for index, case in enumerate(cases):
        write_case(output_directory, index, *case)


if __name__ == "__main__":
    main()
