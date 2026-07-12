#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def delta(l: int, r: int, edges: list[tuple[int, int]]) -> int:
    dl = [0] * l
    dr = [0] * r
    for u, v in edges:
        dl[u] += 1
        dr[v] += 1
    return max([0, *dl, *dr])


def write_case(out_dir: Path, idx: int, l: int, r: int, edges: list[tuple[int, int]]) -> None:
    name = f"case_{idx:02d}"
    (out_dir / f"{name}.in").write_text(
        "\n".join([f"{l} {r} {len(edges)}", *[f"{u} {v}" for u, v in edges]]) + "\n",
        encoding="utf-8",
    )
    (out_dir / f"{name}.out").write_text(f"{delta(l, r, edges)} 1\n", encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()

    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[tuple[int, int, list[tuple[int, int]]]] = [
        (1, 1, []),
        (3, 2, [(0, 0), (0, 0), (0, 1), (1, 0), (1, 1), (2, 1)]),
    ]
    rng = random.Random(20260803)
    for l, r in [(1, 5), (5, 1), (4, 4), (8, 6), (12, 10)]:
        edges: list[tuple[int, int]] = []
        for u in range(l):
            for v in range(r):
                for _ in range(rng.randrange(3)):
                    edges.append((u, v))
        cases.append((l, r, edges))

    active = 64
    sparse_edges = [
        (rng.randrange(active), rng.randrange(active)) for _ in range(2000)
    ]
    cases.append((50_000, 50_000, sparse_edges))

    for i, case in enumerate(cases):
        write_case(out_dir, i, *case)


if __name__ == "__main__":
    main()
