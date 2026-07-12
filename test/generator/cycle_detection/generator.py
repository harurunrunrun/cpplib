#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def has_cycle(n: int, edges: list[tuple[int, int]]) -> int:
    graph = [[] for _ in range(n)]
    for u, v in edges:
        graph[u].append(v)
    color = [0] * n

    def dfs(v: int) -> bool:
        color[v] = 1
        for to in graph[v]:
            if color[to] == 0:
                if dfs(to):
                    return True
            elif color[to] == 1:
                return True
        color[v] = 2
        return False

    return int(any(color[v] == 0 and dfs(v) for v in range(n)))


def write_case(out_dir: Path, idx: int, n: int, edges: list[tuple[int, int]]) -> None:
    name = f"case_{idx:02d}"
    (out_dir / f"{name}.in").write_text(
        "\n".join([f"{n} {len(edges)}", *[f"{u} {v}" for u, v in edges]]) + "\n",
        encoding="utf-8",
    )
    (out_dir / f"{name}.out").write_text(f"{has_cycle(n, edges)}\n", encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()

    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[tuple[int, list[tuple[int, int]]]] = [
        (1, []),
        (3, [(0, 1), (1, 2), (2, 0)]),
        (4, [(0, 1), (1, 2), (2, 3)]),
    ]
    rng = random.Random(20260804)
    for n in [2, 5, 20, 50]:
        edges: list[tuple[int, int]] = []
        for u in range(n):
            for v in range(n):
                if u != v and rng.randrange(9) == 0:
                    edges.append((u, v))
        cases.append((n, edges))
    for n in [8, 30]:
        edges = [(u, v) for u in range(n) for v in range(u + 1, n) if rng.randrange(5) == 0]
        cases.append((n, edges))

    for i, case in enumerate(cases):
        write_case(out_dir, i, *case)


if __name__ == "__main__":
    main()
