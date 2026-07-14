#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path


def solve(rows):
    n = len(rows) + 1
    edges = []
    for v, (a, b, x) in enumerate(rows):
        edges.append((v, v + 1, a))
        edges.append((v, x, b))
    inf = 10 ** 30
    dist = [inf] * n
    dist[0] = 0
    for _ in range(n - 1):
        changed = False
        for source, target, weight in edges:
            if dist[source] + weight < dist[target]:
                dist[target] = dist[source] + weight
                changed = True
        if not changed:
            break
    return dist[-1]


def write_case(out_dir, index, rows):
    lines = [str(len(rows) + 1)] + [f"{a} {b} {x + 1}" for a, b, x in rows]
    (out_dir / f"case_{index:02d}.in").write_text("\n".join(lines) + "\n")
    (out_dir / f"case_{index:02d}.out").write_text(f"{solve(rows)}\n")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    cases = [[(5, 2, 1)], [(10, 1, 0), (10, 1, 2), (10, 1, 3)]]
    rng = random.Random(34004)
    for n in range(2, 20):
        rows = [(rng.randint(1, 30), rng.randint(1, 30), rng.randrange(n))
                for _ in range(n - 1)]
        cases.append(rows)
    for index, rows in enumerate(cases):
        write_case(out_dir, index, rows)


if __name__ == "__main__":
    main()
