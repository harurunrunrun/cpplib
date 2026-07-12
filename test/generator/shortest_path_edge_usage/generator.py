#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import heapq
import random
from pathlib import Path


def dijkstra(n: int, graph: list[list[tuple[int, int]]], s: int) -> list[int | None]:
    dist: list[int | None] = [None] * n
    dist[s] = 0
    que = [(0, s)]
    while que:
        d, v = heapq.heappop(que)
        if dist[v] != d:
            continue
        for to, w in graph[v]:
            nd = d + w
            if dist[to] is None or nd < dist[to]:
                dist[to] = nd
                heapq.heappush(que, (nd, to))
    return dist


def solve(n: int, edges: list[tuple[int, int, int]], s: int, t: int) -> list[int]:
    graph: list[list[tuple[int, int]]] = [[] for _ in range(n)]
    rev: list[list[tuple[int, int]]] = [[] for _ in range(n)]
    for u, v, w in edges:
        graph[u].append((v, w))
        rev[v].append((u, w))
    ds = dijkstra(n, graph, s)
    dt = dijkstra(n, rev, t)
    shortest = ds[t]
    if shortest is None:
        return [0] * len(edges)
    return [
        int(ds[u] is not None and dt[v] is not None and ds[u] + w + dt[v] == shortest)
        for u, v, w in edges
    ]


def write_case(out_dir: Path, idx: int, n: int, edges: list[tuple[int, int, int]], s: int, t: int) -> None:
    name = f"case_{idx:02d}"
    (out_dir / f"{name}.in").write_text(
        "\n".join([f"{n} {len(edges)} {s} {t}", *[f"{u} {v} {w}" for u, v, w in edges]]) + "\n",
        encoding="utf-8",
    )
    (out_dir / f"{name}.out").write_text(" ".join(map(str, solve(n, edges, s, t))) + "\n", encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()

    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    sentinel = 1 << 60
    cases: list[tuple[int, list[tuple[int, int, int]], int, int]] = [
        (1, [], 0, 0),
        (4, [(0, 1, 1), (1, 3, 2), (0, 2, 1), (2, 3, 2), (0, 3, 4)], 0, 3),
        (4, [(0, 1, 1), (2, 3, 1)], 0, 3),
        (3, [(0, 1, sentinel), (1, 2, 2_000_000_000_000_000_000),
             (0, 2, 4_000_000_000_000_000_000)], 0, 2),
        (2, [(0, 1, sentinel)], 0, 1),
    ]
    rng = random.Random(20260802)
    for n in [2, 5, 8, 30]:
        edges: list[tuple[int, int, int]] = []
        for u in range(n):
            for v in range(n):
                if u != v and rng.randrange(5) == 0:
                    edges.append((u, v, 1 + rng.randrange(8)))
        cases.append((n, edges, rng.randrange(n), rng.randrange(n)))

    for i, case in enumerate(cases):
        write_case(out_dir, i, *case)


if __name__ == "__main__":
    main()
