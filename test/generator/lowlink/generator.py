#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def components(n: int, edges: list[tuple[int, int]], banned_vertex: int, banned_edge: int) -> int:
    graph = [[] for _ in range(n)]
    for i, (u, v) in enumerate(edges):
        if i == banned_edge or u == banned_vertex or v == banned_vertex:
            continue
        graph[u].append(v)
        graph[v].append(u)
    seen = [False] * n
    if banned_vertex != -1:
        seen[banned_vertex] = True
    result = 0
    for s in range(n):
        if seen[s]:
            continue
        result += 1
        stack = [s]
        seen[s] = True
        while stack:
            v = stack.pop()
            for to in graph[v]:
                if not seen[to]:
                    seen[to] = True
                    stack.append(to)
    return result


def solve(n: int, edges: list[tuple[int, int]]) -> tuple[int, int]:
    base = components(n, edges, -1, -1)
    articulation = sum(components(n, edges, v, -1) > base for v in range(n))
    bridges = sum(components(n, edges, -1, i) > base for i in range(len(edges)))
    return articulation, bridges


def write_case(out_dir: Path, idx: int, n: int, edges: list[tuple[int, int]]) -> None:
    name = f"case_{idx:02d}"
    (out_dir / f"{name}.in").write_text(
        "\n".join([f"{n} {len(edges)}", *[f"{u} {v}" for u, v in edges]]) + "\n",
        encoding="utf-8",
    )
    a, b = solve(n, edges)
    (out_dir / f"{name}.out").write_text(f"{a} {b}\n", encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()

    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[tuple[int, list[tuple[int, int]]]] = [
        (1, []),
        (4, [(0, 1), (1, 2), (2, 0), (1, 3)]),
        (2, [(0, 1), (0, 1)]),
    ]
    rng = random.Random(20260805)
    for n in [2, 5, 9, 20]:
        edges: list[tuple[int, int]] = []
        for u in range(n):
            for v in range(u + 1, n):
                if rng.randrange(3) == 0:
                    edges.append((u, v))
                if rng.randrange(13) == 0:
                    edges.append((u, v))
        cases.append((n, edges))

    for i, case in enumerate(cases):
        write_case(out_dir, i, *case)


if __name__ == "__main__":
    main()
