#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def solve(n: int, one: list[tuple[int, int]], pair: list[tuple[int, int, int, int, int, int]]) -> int:
    best = 1 << 60
    for mask in range(1 << n):
        cost = 0
        for i, (c0, c1) in enumerate(one):
            cost += c1 if (mask >> i) & 1 else c0
        for i, j, c00, c01, c10, c11 in pair:
            a = (mask >> i) & 1
            b = (mask >> j) & 1
            if a == 0 and b == 0:
                cost += c00
            elif a == 0 and b == 1:
                cost += c01
            elif a == 1 and b == 0:
                cost += c10
            else:
                cost += c11
        best = min(best, cost)
    return best


def write_case(out_dir: Path, idx: int, n: int, one: list[tuple[int, int]], pair: list[tuple[int, int, int, int, int, int]]) -> None:
    name = f"case_{idx:02d}"
    lines = [f"{n} {len(pair)}"]
    lines += [f"{c0} {c1}" for c0, c1 in one]
    lines += [f"{i} {j} {c00} {c01} {c10} {c11}" for i, j, c00, c01, c10, c11 in pair]
    (out_dir / f"{name}.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (out_dir / f"{name}.out").write_text(f"{solve(n, one, pair)}\n", encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()

    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    cases = [
        (0, [], []),
        (2, [(3, 0), (0, 3)], [(0, 1, 0, 5, 5, 0)]),
    ]
    rng = random.Random(20260825)
    for n in [1, 5, 10, 12]:
        one = [(rng.randrange(21) - 10, rng.randrange(21) - 10) for _ in range(n)]
        pair = []
        for i in range(n):
            for j in range(i + 1, n):
                if rng.randrange(7) != 0:
                    continue
                c00 = rng.randrange(11) - 5
                c01 = rng.randrange(11) - 5
                c10 = rng.randrange(11) - 5
                c11 = rng.randrange(11) - 5
                need = c00 + c11 - c01 - c10
                if need > 0:
                    c01 += need
                pair.append((i, j, c00, c01, c10, c11))
        cases.append((n, one, pair))

    for i, case in enumerate(cases):
        write_case(out_dir, i, *case)


if __name__ == "__main__":
    main()
