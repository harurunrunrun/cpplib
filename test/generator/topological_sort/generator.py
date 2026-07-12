#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from collections import deque
from pathlib import Path


def is_dag(n: int, edges: list[tuple[int, int]]) -> int:
    graph = [[] for _ in range(n)]
    indeg = [0] * n
    for u, v in edges:
        graph[u].append(v)
        indeg[v] += 1
    que = deque([v for v in range(n) if indeg[v] == 0])
    cnt = 0
    while que:
        v = que.popleft()
        cnt += 1
        for to in graph[v]:
            indeg[to] -= 1
            if indeg[to] == 0:
                que.append(to)
    return int(cnt == n)


def write_case(out_dir: Path, idx: int, n: int, edges: list[tuple[int, int]]) -> None:
    name = f"case_{idx:02d}"
    (out_dir / f"{name}.in").write_text(
        "\n".join([f"{n} {len(edges)}", *[f"{u} {v}" for u, v in edges]]) + "\n",
        encoding="utf-8",
    )
    (out_dir / f"{name}.out").write_text(f"{is_dag(n, edges)}\n", encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()

    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[tuple[int, list[tuple[int, int]]]] = [
        (1, []),
        (4, [(0, 1), (0, 2), (1, 3), (2, 3)]),
        (2, [(0, 1), (1, 0)]),
    ]
    rng = random.Random(20260809)
    for n in [5, 30, 80]:
        edges = [(u, v) for u in range(n) for v in range(u + 1, n) if rng.randrange(8) == 0]
        cases.append((n, edges))
    for n in [5, 30]:
        edges = [(u, v) for u in range(n) for v in range(n) if u != v and rng.randrange(30) == 0]
        cases.append((n, edges))

    for i, case in enumerate(cases):
        write_case(out_dir, i, *case)


if __name__ == "__main__":
    main()
