#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


class Dsu:
    def __init__(self, n: int) -> None:
        self.p = list(range(n))

    def leader(self, x: int) -> int:
        while self.p[x] != x:
            self.p[x] = self.p[self.p[x]]
            x = self.p[x]
        return x

    def merge(self, a: int, b: int) -> bool:
        a = self.leader(a)
        b = self.leader(b)
        if a == b:
            return False
        self.p[b] = a
        return True


def solve(n: int, edges: list[tuple[int, int, int]]) -> int:
    dsu = Dsu(n)
    cost = 0
    count = 0
    for u, v, w in sorted(edges, key=lambda x: x[2]):
        if dsu.merge(u, v):
            cost += w
            count += 1
    return cost if count == max(0, n - 1) else -1


def write_case(out_dir: Path, idx: int, n: int, edges: list[tuple[int, int, int]]) -> None:
    name = f"case_{idx:02d}"
    (out_dir / f"{name}.in").write_text(
        "\n".join([f"{n} {len(edges)}", *[f"{u} {v} {w}" for u, v, w in edges]]) + "\n",
        encoding="utf-8",
    )
    (out_dir / f"{name}.out").write_text(f"{solve(n, edges)}\n", encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()

    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[tuple[int, list[tuple[int, int, int]]]] = [
        (0, []),
        (4, [(0, 1, 1), (1, 2, 2), (0, 2, 10), (2, 3, 3)]),
        (4, [(0, 1, 1), (2, 3, 1)]),
    ]
    rng = random.Random(20260814)
    for n in [1, 5, 20, 50]:
        edges: list[tuple[int, int, int]] = []
        for u in range(n):
            for v in range(u + 1, n):
                if rng.randrange(4) == 0:
                    edges.append((u, v, rng.randrange(50) - 20))
        cases.append((n, edges))

    for i, case in enumerate(cases):
        write_case(out_dir, i, *case)


if __name__ == "__main__":
    main()
