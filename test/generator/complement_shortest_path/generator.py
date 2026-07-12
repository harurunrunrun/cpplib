#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from collections import deque
from pathlib import Path


def solve(n: int, edges: list[tuple[int, int]], s: int) -> list[int]:
    has = [[False] * n for _ in range(n)]
    for u, v in edges:
        has[u][v] = True
    dist = [-1] * n
    dist[s] = 0
    que: deque[int] = deque([s])
    while que:
        v = que.popleft()
        for to in range(n):
            if v == to or has[v][to] or dist[to] != -1:
                continue
            dist[to] = dist[v] + 1
            que.append(to)
    return dist


def write_case(out_dir: Path, idx: int, n: int, edges: list[tuple[int, int]], s: int) -> None:
    name = f"case_{idx:02d}"
    (out_dir / f"{name}.in").write_text(
        "\n".join([f"{n} {len(edges)} {s}", *[f"{u} {v}" for u, v in edges]]) + "\n",
        encoding="utf-8",
    )
    (out_dir / f"{name}.out").write_text(" ".join(map(str, solve(n, edges, s))) + "\n", encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()

    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[tuple[int, list[tuple[int, int]], int]] = [
        (1, [], 0),
        (4, [(0, 1), (0, 2), (1, 2), (1, 3), (2, 3)], 0),
        (5, [(u, v) for u in range(5) for v in range(5) if u != v], 0),
    ]
    rng = random.Random(20260728)
    for n, mod in [(2, 2), (6, 3), (15, 4), (40, 2), (70, 5)]:
        edges = [(u, v) for u in range(n) for v in range(n) if u != v and rng.randrange(mod) == 0]
        cases.append((n, edges, rng.randrange(n)))

    for i, (n, edges, s) in enumerate(cases):
        write_case(out_dir, i, n, edges, s)


if __name__ == "__main__":
    main()
