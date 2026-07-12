#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import heapq
import random
from pathlib import Path


def solve(n: int, graph: list[list[tuple[int, int]]], s: int) -> list[int]:
    dist: list[int | None] = [None] * n
    dist[s] = 0
    que: list[tuple[int, int]] = [(0, s)]
    while que:
        d, v = heapq.heappop(que)
        if dist[v] != d:
            continue
        for to, cost in graph[v]:
            nd = d + cost
            if dist[to] is None or nd < dist[to]:
                dist[to] = nd
                heapq.heappush(que, (nd, to))
    return [-1 if value is None else value for value in dist]


def write_case(out_dir: Path, idx: int, n: int, edges: list[tuple[int, int, int]], s: int) -> None:
    graph: list[list[tuple[int, int]]] = [[] for _ in range(n)]
    for u, v, w in edges:
        graph[u].append((v, w))
    name = f"case_{idx:02d}"
    (out_dir / f"{name}.in").write_text(
        "\n".join([f"{n} {len(edges)} {s}", *[f"{u} {v} {w}" for u, v, w in edges]]) + "\n",
        encoding="utf-8",
    )
    (out_dir / f"{name}.out").write_text(" ".join(map(str, solve(n, graph, s))) + "\n", encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()

    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    sentinel = 1 << 60
    cases: list[tuple[int, list[tuple[int, int, int]], int]] = [
        (1, [], 0),
        (4, [(0, 1, 3), (0, 2, 10), (1, 2, 4), (2, 3, 2)], 0),
        (5, [(1, 2, 0), (2, 3, 1), (3, 1, 1)], 4),
        (4, [(0, 1, sentinel), (1, 2, 2_000_000_000_000_000_000)], 0),
        (3, [(0, 1, 3_000_000_000_000_000_000), (1, 2, 1)], 0),
    ]
    rng = random.Random(20260727)
    for n in [2, 3, 7, 20, 60]:
        edges: list[tuple[int, int, int]] = []
        for u in range(n):
            for v in range(n):
                if rng.randrange(5) == 0:
                    edges.append((u, v, rng.randrange(30)))
        cases.append((n, edges, rng.randrange(n)))

    for i, (n, edges, s) in enumerate(cases):
        write_case(out_dir, i, n, edges, s)


if __name__ == "__main__":
    main()
