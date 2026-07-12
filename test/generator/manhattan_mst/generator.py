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


def solve(points: list[tuple[int, int]]) -> int:
    n = len(points)
    edges: list[tuple[int, int, int]] = []
    for i in range(n):
        for j in range(i + 1, n):
            cost = abs(points[i][0] - points[j][0]) + abs(points[i][1] - points[j][1])
            edges.append((cost, i, j))
    dsu = Dsu(n)
    result = 0
    for cost, u, v in sorted(edges):
        if dsu.merge(u, v):
            result += cost
    return result


def write_case(out_dir: Path, idx: int, points: list[tuple[int, int]]) -> None:
    name = f"case_{idx:02d}"
    (out_dir / f"{name}.in").write_text(
        "\n".join([str(len(points)), *[f"{x} {y}" for x, y in points]]) + "\n",
        encoding="utf-8",
    )
    (out_dir / f"{name}.out").write_text(f"{solve(points)}\n", encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()

    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[list[tuple[int, int]]] = [
        [],
        [(0, 0)],
        [(0, 0), (1, 2), (3, 1), (-1, 4)],
    ]
    rng = random.Random(20260817)
    for n in [2, 5, 20, 80]:
        points = [(rng.randrange(101) - 50, rng.randrange(101) - 50) for _ in range(n)]
        cases.append(points)

    for i, points in enumerate(cases):
        write_case(out_dir, i, points)


if __name__ == "__main__":
    main()
