#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path


def solve(n, edges):
    adjacency = [0] * n
    for u, v in edges:
        adjacency[u] |= 1 << v
        adjacency[v] |= 1 << u
    answer = 0
    for mask in range(1 << n):
        if mask.bit_count() <= answer:
            continue
        if all((mask & ~(1 << v) & ~adjacency[v]) == 0 for v in range(n) if mask >> v & 1):
            answer = mask.bit_count()
    return answer


def write_case(out_dir, index, n, edges):
    lines = [f"{n} {len(edges)}"] + [f"{u + 1} {v + 1}" for u, v in edges]
    (out_dir / f"case_{index:02d}.in").write_text("\n".join(lines) + "\n")
    (out_dir / f"case_{index:02d}.out").write_text(f"{solve(n, edges)}\n")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    cases = [(1, []), (5, [(i, j) for i in range(5) for j in range(i + 1, 5)]),
             (6, [(i, i + 1) for i in range(5)])]
    rng = random.Random(200204)
    for n in range(2, 10):
        edges = [(i, j) for i in range(n) for j in range(i + 1, n) if rng.randrange(2)]
        cases.append((n, edges))
    for index, case in enumerate(cases):
        write_case(out_dir, index, *case)


if __name__ == "__main__":
    main()
