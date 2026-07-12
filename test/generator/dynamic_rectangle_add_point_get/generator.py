#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def solve(queries: list[str]) -> str:
    grid = [[0 for _ in range(20)] for _ in range(20)]
    out: list[str] = []
    for query in queries:
        xs = query.split()
        if xs[0] == "ADD":
            x1, y1, x2, y2, value = map(int, xs[1:])
            if x1 > x2:
                x1, x2 = x2, x1
            if y1 > y2:
                y1, y2 = y2, y1
            for x in range(x1, x2):
                for y in range(y1, y2):
                    grid[x][y] += value
        else:
            x, y = map(int, xs[1:])
            out.append(str(grid[x][y]))
    return "\n".join(out) + ("\n" if out else "")


def write_case(out_dir: Path, idx: int, queries: list[str]) -> None:
    name = f"case_{idx:02d}"
    (out_dir / f"{name}.in").write_text("\n".join([str(len(queries)), *queries]) + "\n", encoding="utf-8")
    (out_dir / f"{name}.out").write_text(solve(queries), encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()

    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    write_case(out_dir, 0, ["ADD 0 0 3 3 5", "GET 1 1", "ADD 1 1 4 4 -2", "GET 2 2", "GET 3 3"])
    rng = random.Random(20260914)
    queries: list[str] = []
    for _ in range(100):
        if rng.randrange(2) == 0:
            x1, x2 = rng.randrange(20), rng.randrange(20)
            y1, y2 = rng.randrange(20), rng.randrange(20)
            queries.append(f"ADD {x1} {y1} {x2} {y2} {rng.randrange(-10, 11)}")
        else:
            queries.append(f"GET {rng.randrange(20)} {rng.randrange(20)}")
    write_case(out_dir, 1, queries)


if __name__ == "__main__":
    main()
