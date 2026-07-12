#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


Edge = tuple[int, int]


def solve(n: int, edges: list[Edge]) -> int:
    used = [False] * n
    best = 0

    def dfs(i: int, count: int) -> None:
        nonlocal best
        if i == len(edges):
            best = max(best, count)
            return
        dfs(i + 1, count)
        u, v = edges[i]
        if not used[u] and not used[v]:
            used[u] = used[v] = True
            dfs(i + 1, count + 1)
            used[u] = used[v] = False

    dfs(0, 0)
    return best


def make_case(n: int, rng: random.Random) -> list[Edge]:
    if n <= 1:
        return []
    edges: list[Edge] = []
    cycle_size = 2 if n == 2 else rng.randrange(3, n + 1)
    if cycle_size == 2:
        edges.append((0, 1))
        edges.append((0, 1))
    else:
        for i in range(cycle_size):
            edges.append((i, (i + 1) % cycle_size))
    for v in range(cycle_size, n):
        edges.append((rng.randrange(v), v))
    rng.shuffle(edges)
    return edges


def write_case(out_dir: Path, idx: int, n: int, edges: list[Edge]) -> None:
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

    cases: list[tuple[int, list[Edge]]] = [
        (0, []),
        (5, [(0, 1), (1, 2), (2, 0), (3, 4)]),
        (2, [(0, 1), (0, 1)]),
    ]
    rng = random.Random(20260906)
    for n in [1, 2, 5, 9, 11]:
        cases.append((n, make_case(n, rng)))

    for i, case in enumerate(cases):
        write_case(out_dir, i, *case)


if __name__ == "__main__":
    main()
