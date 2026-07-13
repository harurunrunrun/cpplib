#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from collections import deque
from pathlib import Path


def solve(n: int, edges: list[tuple[int, int]], sources: list[int]) -> list[int]:
    graph = [[] for _ in range(n)]
    for u, v in edges:
        graph[u].append(v)
    dist = [-1] * n
    queue: deque[int] = deque()
    for source in sources:
        if dist[source] != -1:
            continue
        dist[source] = 0
        queue.append(source)
    while queue:
        v = queue.popleft()
        for to in graph[v]:
            if dist[to] != -1:
                continue
            dist[to] = dist[v] + 1
            queue.append(to)
    return dist


def write_case(out_dir: Path, index: int, n: int, edges: list[tuple[int, int]], sources: list[int]) -> None:
    name = f"case_{index:02d}"
    lines = [f"{n} {len(edges)} {len(sources)}", " ".join(map(str, sources))]
    lines.extend(f"{u} {v}" for u, v in edges)
    (out_dir / f"{name}.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (out_dir / f"{name}.out").write_text(" ".join(map(str, solve(n, edges, sources))) + "\n", encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[tuple[int, list[tuple[int, int]], list[int]]] = [
        (1, [], []),
        (1, [(0, 0)], [0, 0]),
        (6, [(0, 1), (0, 2), (3, 2), (3, 4), (2, 5)], [0, 3, 0]),
        (5, [(0, 1), (1, 2), (3, 4)], [3]),
    ]
    rng = random.Random(20260830)
    for n in [2, 7, 40, 120]:
        edges = [
            (u, v)
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
