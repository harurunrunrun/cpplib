#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def solve(n: int, edges: list[tuple[int, int, int]]) -> tuple[int, int]:
    exists = [[False] * n for _ in range(n)]
    cost = [[10**18] * n for _ in range(n)]
    for u, v, c in edges:
        exists[u][v] = exists[v][u] = True
        cost[u][v] = min(cost[u][v], c)
        cost[v][u] = min(cost[v][u], c)
    used = [False] * n
    best_size = 0
    best_cost = 0

    def dfs(size: int, total: int) -> None:
        nonlocal best_size, best_cost
        v = next((i for i in range(n) if not used[i]), -1)
        if v == -1:
            if best_size < size or (best_size == size and total < best_cost):
                best_size = size
                best_cost = total
            return
        used[v] = True
        dfs(size, total)
        for u in range(v + 1, n):
            if not used[u] and exists[v][u]:
                used[u] = True
                dfs(size + 1, total + cost[v][u])
                used[u] = False
        used[v] = False

    dfs(0, 0)
    return best_size, best_cost


def write_case(out_dir: Path, idx: int, n: int, edges: list[tuple[int, int, int]]) -> None:
    name = f"case_{idx:02d}"
    (out_dir / f"{name}.in").write_text(
        "\n".join([f"{n} {len(edges)}", *[f"{u} {v} {c}" for u, v, c in edges]]) + "\n",
        encoding="utf-8",
    )
    size, cost = solve(n, edges)
    (out_dir / f"{name}.out").write_text(f"{size} {cost}\n", encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()

    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[tuple[int, list[tuple[int, int, int]]]] = [
        (0, []),
        (5, [(0, 1, 5), (1, 2, 1), (2, 0, 1), (3, 4, -2)]),
    ]
    rng = random.Random(20260829)
    for n in [1, 5, 9, 12]:
        edges = [(u, v, rng.randrange(21) - 10) for u in range(n) for v in range(u + 1, n) if rng.randrange(4) == 0]
        cases.append((n, edges))

    for i, case in enumerate(cases):
        write_case(out_dir, i, *case)


if __name__ == "__main__":
    main()
