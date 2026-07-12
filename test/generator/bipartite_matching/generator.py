#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def solve(l: int, r: int, edges: list[tuple[int, int]]) -> int:
    graph = [[] for _ in range(l)]
    for u, v in edges:
        graph[u].append(v)
    used = [False] * r
    best = 0

    def dfs(v: int, count: int) -> None:
        nonlocal best
        if v == l:
            best = max(best, count)
            return
        dfs(v + 1, count)
        for to in graph[v]:
            if not used[to]:
                used[to] = True
                dfs(v + 1, count + 1)
                used[to] = False

    dfs(0, 0)
    return best


def write_case(out_dir: Path, idx: int, l: int, r: int, edges: list[tuple[int, int]]) -> None:
    name = f"case_{idx:02d}"
    (out_dir / f"{name}.in").write_text(
        "\n".join([f"{l} {r} {len(edges)}", *[f"{u} {v}" for u, v in edges]]) + "\n",
        encoding="utf-8",
    )
    (out_dir / f"{name}.out").write_text(f"{solve(l, r, edges)}\n", encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()

    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[tuple[int, int, list[tuple[int, int]]]] = [
        (0, 0, []),
        (3, 3, [(0, 0), (0, 1), (1, 1), (2, 1), (2, 2)]),
    ]
    rng = random.Random(20260822)
    for l, r in [(1, 5), (5, 1), (5, 5), (9, 9)]:
        edges = [(i, j) for i in range(l) for j in range(r) if rng.randrange(3) == 0]
        cases.append((l, r, edges))

    for i, case in enumerate(cases):
        write_case(out_dir, i, *case)


if __name__ == "__main__":
    main()
