#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


class DSU:
    def __init__(self, n: int) -> None:
        self.parent = list(range(n))
        self.size = [1] * n

    def leader(self, x: int) -> int:
        while self.parent[x] != x:
            self.parent[x] = self.parent[self.parent[x]]
            x = self.parent[x]
        return x

    def unite(self, x: int, y: int) -> None:
        x = self.leader(x)
        y = self.leader(y)
        if x == y:
            return
        if self.size[x] < self.size[y]:
            x, y = y, x
        self.parent[y] = x
        self.size[x] += self.size[y]

    def same(self, x: int, y: int) -> bool:
        return self.leader(x) == self.leader(y)

    def component_size(self, x: int) -> int:
        return self.size[self.leader(x)]


def solve(n: int, queries: list[str]) -> str:
    dsu = DSU(n)
    out: list[str] = []
    for query in queries:
        xs = query.split()
        if xs[0] == "UNITE":
            dsu.unite(int(xs[1]), int(xs[2]))
        elif xs[0] == "RANGE":
            l, r, x = map(int, xs[1:])
            for i in range(l, r):
                dsu.unite(i, x)
        elif xs[0] == "SAME":
            out.append(str(int(dsu.same(int(xs[1]), int(xs[2])))))
        elif xs[0] == "SIZE":
            out.append(str(dsu.component_size(int(xs[1]))))
    return "\n".join(out) + ("\n" if out else "")


def write_case(out_dir: Path, idx: int, n: int, queries: list[str]) -> None:
    name = f"case_{idx:02d}"
    (out_dir / f"{name}.in").write_text("\n".join([f"{n} {len(queries)}", *queries]) + "\n", encoding="utf-8")
    (out_dir / f"{name}.out").write_text(solve(n, queries), encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()

    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    fixed = [
        "RANGE 1 4 1",
        "SAME 1 3",
        "SAME 0 1",
        "RANGE 3 6 7",
        "SAME 1 7",
        "SIZE 7",
    ]
    write_case(out_dir, 0, 8, fixed)

    rng = random.Random(20260909)
    n = 30
    queries: list[str] = []
    for _ in range(200):
        typ = rng.choice(["UNITE", "RANGE", "SAME", "SIZE"])
        if typ == "UNITE":
            queries.append(f"UNITE {rng.randrange(n)} {rng.randrange(n)}")
        elif typ == "RANGE":
            l = rng.randrange(n + 1)
            r = rng.randrange(n + 1)
            if l > r:
                l, r = r, l
            queries.append(f"RANGE {l} {r} {rng.randrange(n)}")
        elif typ == "SAME":
            queries.append(f"SAME {rng.randrange(n)} {rng.randrange(n)}")
        else:
            queries.append(f"SIZE {rng.randrange(n)}")
    write_case(out_dir, 1, n, queries)


if __name__ == "__main__":
    main()
