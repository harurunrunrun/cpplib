#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


Initial = tuple[int, int, int]


def solve(initial: list[Initial], queries: list[str]) -> str:
    points: dict[tuple[int, int], int] = {}
    for x, y, value in initial:
        points[(x, y)] = points.get((x, y), 0) + value
    out: list[str] = []
    for query in queries:
        xs = query.split()
        if xs[0] == "ADD":
            x, y, value = map(int, xs[1:])
            points[(x, y)] = points.get((x, y), 0) + value
        else:
            x1, y1, x2, y2 = map(int, xs[1:])
            out.append(str(sum(value for (x, y), value in points.items() if x1 <= x < x2 and y1 <= y < y2)))
    return "\n".join(out) + ("\n" if out else "")


def write_case(out_dir: Path, idx: int, initial: list[Initial], queries: list[str]) -> None:
    name = f"case_{idx:02d}"
    lines = [f"{len(initial)} {len(queries)}"]
    lines += [f"{x} {y} {value}" for x, y, value in initial]
    lines += queries
    (out_dir / f"{name}.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (out_dir / f"{name}.out").write_text(solve(initial, queries), encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()

    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    write_case(out_dir, 0, [(1, 2, 5), (3, 4, 7)], ["SUM 0 0 5 5", "ADD 1 2 -2", "SUM 0 0 2 3"])
    rng = random.Random(20260913)
    initial = [(rng.randrange(-10, 10), rng.randrange(-10, 10), rng.randrange(-10, 11)) for _ in range(10)]
    queries: list[str] = []
    for _ in range(100):
        if rng.randrange(2) == 0:
            queries.append(f"ADD {rng.randrange(-10, 10)} {rng.randrange(-10, 10)} {rng.randrange(-10, 11)}")
        else:
            x1, x2 = rng.randrange(-10, 10), rng.randrange(-10, 10)
            y1, y2 = rng.randrange(-10, 10), rng.randrange(-10, 10)
            if x1 > x2:
                x1, x2 = x2, x1
            if y1 > y2:
                y1, y2 = y2, y1
            queries.append(f"SUM {x1} {y1} {x2} {y2}")
    write_case(out_dir, 1, initial, queries)


if __name__ == "__main__":
    main()
