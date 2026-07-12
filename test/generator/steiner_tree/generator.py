#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path

INF = 1 << 60


class Dsu:
    def __init__(self, n: int) -> None:
        self.p = list(range(n))

    def leader(self, x: int) -> int:
        while self.p[x] != x:
            self.p[x] = self.p[self.p[x]]
            x = self.p[x]
        return x

    def merge(self, a: int, b: int) -> None:
        a = self.leader(a)
        b = self.leader(b)
        if a != b:
            self.p[b] = a


def solve(n: int, edges: list[tuple[int, int, int]], terminals: list[int]) -> int:
    if not terminals:
        return 0
    best: int | None = None
    m = len(edges)
    for mask in range(1 << m):
        dsu = Dsu(n)
        cost = 0
        for i, (u, v, w) in enumerate(edges):
            if (mask >> i) & 1:
                dsu.merge(u, v)
                cost += w
        root = dsu.leader(terminals[0])
        if all(dsu.leader(t) == root for t in terminals):
            best = cost if best is None else min(best, cost)
    return -1 if best is None else best


def write_case(out_dir: Path, idx: int, n: int, edges: list[tuple[int, int, int]], terminals: list[int]) -> None:
    name = f"case_{idx:02d}"
    (out_dir / f"{name}.in").write_text(
        "\n".join([
            f"{n} {len(edges)} {len(terminals)}",
            " ".join(map(str, terminals)),
            *[f"{u} {v} {w}" for u, v, w in edges],
        ]) + "\n",
        encoding="utf-8",
    )
    (out_dir / f"{name}.out").write_text(f"{solve(n, edges, terminals)}\n", encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()

    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[tuple[int, list[tuple[int, int, int]], list[int]]] = [
        (1, [], []),
        (5, [(0, 1, 2), (1, 2, 2), (1, 3, 1), (3, 4, 1), (2, 4, 10)], [0, 2, 4]),
        (4, [(0, 1, 1), (2, 3, 1)], [0, 3]),
        (3, [(0, 1, INF), (1, 2, 2_000_000_000_000_000_000)], [0, 2]),
    ]
    rng = random.Random(20260808)
    for n in [2, 5, 8]:
        edges: list[tuple[int, int, int]] = []
        for u in range(n):
            for v in range(u + 1, n):
                if rng.randrange(4) == 0 and len(edges) < 14:
                    edges.append((u, v, 1 + rng.randrange(8)))
        terminals = [v for v in range(n) if rng.randrange(4) == 0][:4]
        cases.append((n, edges, terminals))

    for i, case in enumerate(cases):
        write_case(out_dir, i, *case)


if __name__ == "__main__":
    main()
