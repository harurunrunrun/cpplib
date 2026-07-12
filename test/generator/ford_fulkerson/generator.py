#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def solve(n: int, s: int, t: int, edges: list[tuple[int, int, int]]) -> int:
    if s == t:
        return 0
    best = 1 << 60
    for mask in range(1 << n):
        if not ((mask >> s) & 1) or ((mask >> t) & 1):
            continue
        cut = 0
        for u, v, c in edges:
            if ((mask >> u) & 1) and not ((mask >> v) & 1):
                cut += c
        best = min(best, cut)
    return best


def write_case(out_dir: Path, idx: int, n: int, s: int, t: int, edges: list[tuple[int, int, int]]) -> None:
    name = f"case_{idx:02d}"
    (out_dir / f"{name}.in").write_text(
        "\n".join([f"{n} {len(edges)} {s} {t}", *[f"{u} {v} {c}" for u, v, c in edges]]) + "\n",
        encoding="utf-8",
    )
    (out_dir / f"{name}.out").write_text(f"{solve(n, s, t, edges)}\n", encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()

    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[tuple[int, int, int, list[tuple[int, int, int]]]] = [
        (1, 0, 0, [(0, 0, 7)]),
        (2, 0, 1, []),
        (4, 0, 3, [(0, 0, 100), (0, 1, 3), (0, 2, 2),
                    (1, 2, 1), (1, 3, 2), (2, 3, 4), (3, 3, 100)]),
    ]
    rng = random.Random(20260826)
    for n in [2, 5, 8, 10]:
        edges: list[tuple[int, int, int]] = []
        for u in range(n):
            for v in range(n):
                if u != v and rng.randrange(5) == 0:
                    edges.append((u, v, rng.randrange(6)))
        cases.append((n, 0, n - 1, edges))

    for i, case in enumerate(cases):
        write_case(out_dir, i, *case)


if __name__ == "__main__":
    main()
