#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def components(n: int, edges: list[tuple[int, int]], banned: int) -> int:
    graph = [[] for _ in range(n)]
    for i, (u, v) in enumerate(edges):
        if i == banned:
            continue
        graph[u].append(v)
        graph[v].append(u)
    seen = [False] * n
    count = 0
    for s in range(n):
        if seen[s]:
            continue
        count += 1
        stack = [s]
        seen[s] = True
        while stack:
            v = stack.pop()
            for to in graph[v]:
                if not seen[to]:
                    seen[to] = True
                    stack.append(to)
    return count


def solve(n: int, edges: list[tuple[int, int]]) -> int:
    base = components(n, edges, -1)
    bridges = {i for i in range(len(edges)) if components(n, edges, i) > base}
    graph = [[] for _ in range(n)]
    for i, (u, v) in enumerate(edges):
        if i in bridges:
            continue
        graph[u].append(v)
        graph[v].append(u)
    seen = [False] * n
    count = 0
    for s in range(n):
        if seen[s]:
            continue
        count += 1
        stack = [s]
        seen[s] = True
        while stack:
            v = stack.pop()
            for to in graph[v]:
                if not seen[to]:
                    seen[to] = True
                    stack.append(to)
    return count


def write_case(out_dir: Path, idx: int, n: int, edges: list[tuple[int, int]]) -> None:
    name = f"case_{idx:02d}"
    (out_dir / f"{name}.in").write_text(
        "\n".join([f"{n} {len(edges)}", *[f"{u} {v}" for u, v in edges]]) + "\n",
        encoding="utf-8",
    )
    (out_dir / f"{name}.out").write_text(f"{solve(n, edges)}\n", encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()

    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[tuple[int, list[tuple[int, int]]]] = [
        (0, []),
        (4, [(0, 1), (1, 2), (2, 0), (1, 3)]),
        (2, [(0, 1), (0, 1)]),
    ]
    rng = random.Random(20260818)
    for n in [1, 5, 20, 40]:
        edges: list[tuple[int, int]] = []
        for u in range(n):
            for v in range(u + 1, n):
                if rng.randrange(4) == 0:
                    edges.append((u, v))
                if rng.randrange(19) == 0:
                    edges.append((u, v))
        cases.append((n, edges))

    for i, case in enumerate(cases):
        write_case(out_dir, i, *case)


if __name__ == "__main__":
    main()
