#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path

INF = 1 << 60


def solve(n: int, root: int, edges: list[tuple[int, int, int]]) -> int:
    m = len(edges)
    best: int | None = None
    for mask in range(1 << m):
        if mask.bit_count() != n - 1:
            continue
        indeg = [0] * n
        graph = [[] for _ in range(n)]
        cost = 0
        for i, (u, v, w) in enumerate(edges):
            if (mask >> i) & 1:
                indeg[v] += 1
                graph[u].append(v)
                cost += w
        if indeg[root] != 0:
            continue
        if any(indeg[v] != 1 for v in range(n) if v != root):
            continue
        seen = [False] * n
        stack = [root]
        seen[root] = True
        while stack:
            v = stack.pop()
            for to in graph[v]:
                if not seen[to]:
                    seen[to] = True
                    stack.append(to)
        if all(seen):
            best = cost if best is None else min(best, cost)
    return -1 if best is None else best


def write_case(out_dir: Path, idx: int, n: int, root: int, edges: list[tuple[int, int, int]]) -> None:
    name = f"case_{idx:02d}"
    (out_dir / f"{name}.in").write_text(
        "\n".join([f"{n} {len(edges)} {root}", *[f"{u} {v} {w}" for u, v, w in edges]]) + "\n",
        encoding="utf-8",
    )
    (out_dir / f"{name}.out").write_text(f"{solve(n, root, edges)}\n", encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()

    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[tuple[int, int, list[tuple[int, int, int]]]] = [
        (1, 0, []),
        (4, 0, [(0, 1, 5), (0, 2, 3), (2, 1, 1), (1, 3, 2), (2, 3, 10)]),
        (2, 0, [(1, 0, 1)]),
        (2, 0, [(0, 1, INF)]),
        (3, 0, [(0, 1, INF), (0, 2, 3_000_000_000_000_000_000)]),
    ]
    rng = random.Random(20260816)
    for n in [2, 4, 7]:
        edges: list[tuple[int, int, int]] = []
        for u in range(n):
            for v in range(n):
                if u != v and rng.randrange(3) == 0 and len(edges) < 20:
                    edges.append((u, v, rng.randrange(30) - 10))
        cases.append((n, rng.randrange(n), edges))

    for i, case in enumerate(cases):
        write_case(out_dir, i, *case)


if __name__ == "__main__":
    main()
