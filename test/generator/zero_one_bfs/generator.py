#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import heapq
import random
from pathlib import Path


def solve(n: int, edges: list[tuple[int, int, int]], sources: list[int]) -> list[int]:
    graph = [[] for _ in range(n)]
    for u, v, cost in edges:
        graph[u].append((v, cost))
    inf = 10**9
    dist = [inf] * n
    queue: list[tuple[int, int]] = []
    for source in sources:
        if dist[source] == 0:
            continue
        dist[source] = 0
        heapq.heappush(queue, (0, source))
    while queue:
        distance, v = heapq.heappop(queue)
        if dist[v] != distance:
            continue
        for to, cost in graph[v]:
            next_distance = distance + cost
            if next_distance >= dist[to]:
                continue
            dist[to] = next_distance
            heapq.heappush(queue, (next_distance, to))
    return [-1 if distance == inf else distance for distance in dist]


def write_case(out_dir: Path, index: int, n: int, edges: list[tuple[int, int, int]], sources: list[int]) -> None:
    name = f"case_{index:02d}"
    lines = [f"{n} {len(edges)} {len(sources)}", " ".join(map(str, sources))]
    lines.extend(f"{u} {v} {cost}" for u, v, cost in edges)
    (out_dir / f"{name}.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (out_dir / f"{name}.out").write_text(" ".join(map(str, solve(n, edges, sources))) + "\n", encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[tuple[int, list[tuple[int, int, int]], list[int]]] = [
        (1, [], []),
        (1, [(0, 0, 0), (0, 0, 1)], [0, 0]),
        (6, [(0, 1, 1), (0, 2, 0), (2, 1, 0), (2, 4, 1), (3, 4, 0), (4, 5, 1)], [0, 3, 3]),
        (5, [(0, 1, 1), (1, 2, 0), (3, 4, 0)], [0]),
    ]
    rng = random.Random(20260831)
    for n in [2, 7, 40, 120]:
        edges = [
            (u, v, rng.randrange(2))
            for u in range(n)
            for v in range(n)
            if rng.randrange(11) == 0
        ]
        sources = [v for v in range(n) if rng.randrange(6) == 0]
        if sources:
            sources.append(sources[0])
        cases.append((n, edges, sources))
    for index, case in enumerate(cases):
        write_case(out_dir, index, *case)


if __name__ == "__main__":
    main()
