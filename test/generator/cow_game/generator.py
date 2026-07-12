#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path

INF = 1 << 60


def solve(n: int, source: int, target: int, constraints: list[tuple[int, int, int]]) -> str:
    dist = [0] * n
    for it in range(n):
        updated = False
        for u, v, c in constraints:
            if dist[u] + c < dist[v]:
                dist[v] = dist[u] + c
                updated = True
        if not updated:
            break
        if it == n - 1:
            return "infeasible"

    dist = [INF] * n
    dist[source] = 0
    for _ in range(n - 1):
        updated = False
        for u, v, c in constraints:
            if dist[u] == INF:
                continue
            if dist[u] + c < dist[v]:
                dist[v] = dist[u] + c
                updated = True
        if not updated:
            break
    return "unbounded" if dist[target] == INF else str(dist[target])


def write_case(out_dir: Path, idx: int, n: int, source: int, target: int, constraints: list[tuple[int, int, int]]) -> None:
    name = f"case_{idx:02d}"
    (out_dir / f"{name}.in").write_text(
        "\n".join([f"{n} {len(constraints)} {source} {target}", *[f"{u} {v} {c}" for u, v, c in constraints]]) + "\n",
        encoding="utf-8",
    )
    (out_dir / f"{name}.out").write_text(f"{solve(n, source, target, constraints)}\n", encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()

    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[tuple[int, int, int, list[tuple[int, int, int]]]] = [
        (3, 0, 2, [(0, 1, 3), (1, 2, 4), (0, 2, 10)]),
        (2, 0, 1, [(0, 1, 5), (1, 0, -10)]),
        (2, 0, 1, []),
    ]
    rng = random.Random(20260821)
    for n in [1, 5, 20, 40]:
        constraints: list[tuple[int, int, int]] = []
        for u in range(n):
            for v in range(n):
                if u != v and rng.randrange(7) == 0:
                    constraints.append((u, v, rng.randrange(21) - 10))
        cases.append((n, rng.randrange(n), rng.randrange(n), constraints))

    for i, case in enumerate(cases):
        write_case(out_dir, i, *case)


if __name__ == "__main__":
    main()
