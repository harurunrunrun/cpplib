#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import heapq
import random
from pathlib import Path


def solve(n: int, edges: list[tuple[int, int, int]], sources: list[int]) -> list[int]:
    graph: list[list[tuple[int, int]]] = [[] for _ in range(n)]
    for u, v, w in edges:
        graph[u].append((v, w))
    dist: list[int | None] = [None] * n
    que: list[tuple[int, int]] = []
    for s in sources:
        if dist[s] is None:
            dist[s] = 0
            heapq.heappush(que, (0, s))
    while que:
        d, v = heapq.heappop(que)
        if dist[v] != d:
            continue
        for to, w in graph[v]:
            nd = d + w
            if dist[to] is None or nd < dist[to]:
                dist[to] = nd
                heapq.heappush(que, (nd, to))
    return [-1 if value is None else value for value in dist]


def write_case(out_dir: Path, idx: int, n: int, edges: list[tuple[int, int, int]], sources: list[int]) -> None:
    name = f"case_{idx:02d}"
    (out_dir / f"{name}.in").write_text(
        "\n".join([
            f"{n} {len(edges)} {len(sources)}",
            " ".join(map(str, sources)),
            *[f"{u} {v} {w}" for u, v, w in edges],
        ]) + "\n",
        encoding="utf-8",
    )
    (out_dir / f"{name}.out").write_text(" ".join(map(str, solve(n, edges, sources))) + "\n", encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()

    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    sentinel = 1 << 60
    cases: list[tuple[int, list[tuple[int, int, int]], list[int]]] = [
        (1, [], []),
        (5, [(0, 1, 10), (2, 1, 3), (1, 3, 4), (4, 3, 1)], [0, 2, 2, 4]),
        (4, [(0, 1, sentinel), (1, 2, 2_000_000_000_000_000_000)], [0]),
        (4, [(0, 1, 3_000_000_000_000_000_000), (2, 1, 2)], [0, 2]),
    ]
    rng = random.Random(20260801)
    for n in [2, 3, 10, 40]:
        edges: list[tuple[int, int, int]] = []
        for u in range(n):
            for v in range(n):
                if rng.randrange(5) == 0:
                    edges.append((u, v, rng.randrange(20)))
        sources = [v for v in range(n) if rng.randrange(4) == 0]
        cases.append((n, edges, sources))

    for i, (n, edges, sources) in enumerate(cases):
        write_case(out_dir, i, n, edges, sources)


if __name__ == "__main__":
    main()
