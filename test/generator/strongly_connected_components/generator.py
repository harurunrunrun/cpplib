#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def solve(n: int, edges: list[tuple[int, int]]) -> int:
    graph = [[] for _ in range(n)]
    for u, v in edges:
        graph[u].append(v)
    reach = [[False] * n for _ in range(n)]
    for s in range(n):
        stack = [s]
        reach[s][s] = True
        while stack:
            v = stack.pop()
            for to in graph[v]:
                if not reach[s][to]:
                    reach[s][to] = True
                    stack.append(to)
    used = [False] * n
    count = 0
    for v in range(n):
        if used[v]:
            continue
        count += 1
        for u in range(n):
            if reach[v][u] and reach[u][v]:
                used[u] = True
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
        (5, [(0, 1), (1, 2), (2, 0), (2, 3), (3, 4), (4, 3)]),
    ]
    rng = random.Random(20260810)
    for n in [1, 5, 15, 30]:
        edges = [(u, v) for u in range(n) for v in range(n) if u != v and rng.randrange(8) == 0]
        cases.append((n, edges))

    for i, case in enumerate(cases):
        write_case(out_dir, i, *case)


if __name__ == "__main__":
    main()
