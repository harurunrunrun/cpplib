#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def solve(n: int, clauses: list[tuple[int, int, int, int]]) -> int:
    for mask in range(1 << n):
        ok = True
        for i, f, j, g in clauses:
            if (((mask >> i) & 1) == f) or (((mask >> j) & 1) == g):
                continue
            ok = False
            break
        if ok:
            return 1
    return 0


def write_case(out_dir: Path, idx: int, n: int, clauses: list[tuple[int, int, int, int]]) -> None:
    name = f"case_{idx:02d}"
    (out_dir / f"{name}.in").write_text(
        "\n".join([f"{n} {len(clauses)}", *[f"{i} {f} {j} {g}" for i, f, j, g in clauses]]) + "\n",
        encoding="utf-8",
    )
    (out_dir / f"{name}.out").write_text(f"{solve(n, clauses)}\n", encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()

    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[tuple[int, list[tuple[int, int, int, int]]]] = [
        (0, []),
        (2, [(0, 1, 1, 1), (0, 0, 1, 1), (1, 0, 0, 1)]),
        (1, [(0, 1, 0, 1), (0, 0, 0, 0)]),
    ]
    rng = random.Random(20260811)
    for n in [1, 4, 8, 10]:
        clauses: list[tuple[int, int, int, int]] = []
        for _ in range(25):
            clauses.append((rng.randrange(n), rng.randrange(2), rng.randrange(n), rng.randrange(2)))
        cases.append((n, clauses))

    for i, case in enumerate(cases):
        write_case(out_dir, i, *case)


if __name__ == "__main__":
    main()
