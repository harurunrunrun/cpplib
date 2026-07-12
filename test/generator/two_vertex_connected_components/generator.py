#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def solve(n: int, edges: list[tuple[int, int]]) -> int:
    graph = [[] for _ in range(n)]
    degree = [0] * n
    for i, (u, v) in enumerate(edges):
        graph[u].append((v, i))
        graph[v].append((u, i))
        degree[u] += 1
        degree[v] += 1
    ord_ = [-1] * n
    low = [-1] * n
    stack: list[int] = []
    timer = 0
    count = 0

    def dfs(v: int, pe: int) -> None:
        nonlocal timer, count
        ord_[v] = low[v] = timer
        timer += 1
        for to, eid in graph[v]:
            if eid == pe:
                continue
            if ord_[to] == -1:
                stack.append(eid)
                dfs(to, eid)
                low[v] = min(low[v], low[to])
                if ord_[v] <= low[to]:
                    count += 1
                    while True:
                        x = stack.pop()
                        if x == eid:
                            break
            elif ord_[to] < ord_[v]:
                stack.append(eid)
                low[v] = min(low[v], ord_[to])

    for v in range(n):
        if ord_[v] != -1:
            continue
        if degree[v] == 0:
            ord_[v] = low[v] = timer
            timer += 1
            count += 1
        else:
            dfs(v, -1)
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
        (6, [(0, 1), (1, 2), (2, 0), (1, 3), (3, 4), (4, 1)]),
        (3, [(0, 1), (0, 1)]),
    ]
    rng = random.Random(20260819)
    for n in [1, 5, 20, 40]:
        edges: list[tuple[int, int]] = []
        for u in range(n):
            for v in range(u + 1, n):
                if rng.randrange(4) == 0:
                    edges.append((u, v))
                if rng.randrange(23) == 0:
                    edges.append((u, v))
        cases.append((n, edges))

    for i, case in enumerate(cases):
        write_case(out_dir, i, *case)


if __name__ == "__main__":
    main()
