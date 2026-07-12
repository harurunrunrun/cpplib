#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def solve(n: int, edges: list[tuple[int, int]]) -> int:
    adj = [[False] * n for _ in range(n)]
    for u, v in edges:
        adj[u][v] = adj[v][u] = True

    def dfs(colors: int, color: list[int], v: int) -> bool:
        if v == n:
            return True
        for c in range(colors):
            if all(color[to] != c for to in range(v) if adj[v][to]):
                color[v] = c
                if dfs(colors, color, v + 1):
                    return True
                color[v] = -1
        return False

    for colors in range(n + 1):
        if dfs(colors, [-1] * n, 0):
            return colors
    return n


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
        (5, [(0, 1), (1, 2), (2, 3), (3, 4), (4, 0)]),
        (4, [(0, 1), (0, 2), (0, 3), (1, 2), (1, 3), (2, 3)]),
    ]
    rng = random.Random(20260813)
    for n in [1, 5, 9, 12]:
        edges: list[tuple[int, int]] = []
        for u in range(n):
            for v in range(u + 1, n):
                if rng.randrange(3) == 0:
                    edges.append((u, v))
        cases.append((n, edges))

    for i, case in enumerate(cases):
        write_case(out_dir, i, *case)


if __name__ == "__main__":
    main()
