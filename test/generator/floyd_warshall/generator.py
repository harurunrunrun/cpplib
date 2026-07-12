#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path

INF = 1 << 60


def solve(n: int, edges: list[tuple[int, int, int]]) -> list[list[int]]:
    dist = [[INF] * n for _ in range(n)]
    for i in range(n):
        dist[i][i] = 0
    for u, v, w in edges:
        dist[u][v] = min(dist[u][v], w)
    for k in range(n):
        for i in range(n):
            if dist[i][k] == INF:
                continue
            for j in range(n):
                if dist[k][j] == INF:
                    continue
                dist[i][j] = min(dist[i][j], dist[i][k] + dist[k][j])
    return dist


def format_dist(dist: list[list[int]]) -> str:
    return "\n".join(" ".join("INF" if x == INF else str(x) for x in row) for row in dist) + "\n"


def write_case(out_dir: Path, idx: int, n: int, edges: list[tuple[int, int, int]]) -> None:
    name = f"case_{idx:02d}"
    (out_dir / f"{name}.in").write_text(
        "\n".join([f"{n} {len(edges)}", *[f"{u} {v} {w}" for u, v, w in edges]]) + "\n",
        encoding="utf-8",
    )
    (out_dir / f"{name}.out").write_text(format_dist(solve(n, edges)), encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()

    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[tuple[int, list[tuple[int, int, int]]]] = [
        (1, []),
        (4, [(0, 1, 3), (0, 2, 10), (1, 2, -2), (2, 3, 4)]),
        (4, [(0, 1, 5), (2, 3, 7)]),
    ]
    rng = random.Random(20260731)
    for n in [2, 5, 9, 20]:
        edges: list[tuple[int, int, int]] = []
        for u in range(n):
            for v in range(u + 1, n):
                if rng.randrange(3) == 0:
                    edges.append((u, v, rng.randrange(15) - 5))
        cases.append((n, edges))

    for i, (n, edges) in enumerate(cases):
        write_case(out_dir, i, n, edges)


if __name__ == "__main__":
    main()
