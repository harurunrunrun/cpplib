#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def solve(l: int, r: int, left: list[int], right: list[int], edges: list[tuple[int, int, int]]) -> int:
    n = l + r + 2
    source = l + r
    sink = source + 1
    all_edges: list[tuple[int, int, int]] = []
    for i, c in enumerate(left):
        all_edges.append((source, i, c))
    for u, v, c in edges:
        all_edges.append((u, l + v, c))
    for i, c in enumerate(right):
        all_edges.append((l + i, sink, c))
    best = 1 << 60
    for mask in range(1 << n):
        if not ((mask >> source) & 1) or ((mask >> sink) & 1):
            continue
        cut = 0
        for u, v, c in all_edges:
            if ((mask >> u) & 1) and not ((mask >> v) & 1):
                cut += c
        best = min(best, cut)
    return best


def write_case(out_dir: Path, idx: int, l: int, r: int, left: list[int], right: list[int], edges: list[tuple[int, int, int]]) -> None:
    name = f"case_{idx:02d}"
    lines = [f"{l} {r} {len(edges)}", " ".join(map(str, left)), " ".join(map(str, right))]
    lines += [f"{u} {v} {c}" for u, v, c in edges]
    (out_dir / f"{name}.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (out_dir / f"{name}.out").write_text(f"{solve(l, r, left, right, edges)}\n", encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()

    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    cases = [
        (0, 0, [], [], []),
        (2, 2, [3, 2], [2, 4], [(0, 0, 2), (0, 1, 3), (1, 1, 2)]),
    ]
    rng = random.Random(20260823)
    for l, r in [(1, 5), (5, 1), (4, 4), (5, 5)]:
        left = [rng.randrange(8) for _ in range(l)]
        right = [rng.randrange(8) for _ in range(r)]
        edges = [(i, j, rng.randrange(8)) for i in range(l) for j in range(r) if rng.randrange(3) == 0]
        cases.append((l, r, left, right, edges))

    for i, case in enumerate(cases):
        write_case(out_dir, i, *case)


if __name__ == "__main__":
    main()
