#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def solve(n: int, clauses: list[list[tuple[int, int]]]) -> int:
    for mask in range(1 << n):
        ok = True
        for clause in clauses:
            if not any(((mask >> v) & 1) == value for v, value in clause):
                ok = False
                break
        if ok:
            return 1
    return 0


def write_case(out_dir: Path, idx: int, n: int, clauses: list[list[tuple[int, int]]]) -> None:
    name = f"case_{idx:02d}"
    lines = [f"{n} {len(clauses)}"]
    for clause in clauses:
        lines.append(" ".join(f"{v} {value}" for v, value in clause))
    (out_dir / f"{name}.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (out_dir / f"{name}.out").write_text(f"{solve(n, clauses)}\n", encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()

    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[tuple[int, list[list[tuple[int, int]]]]] = [
        (0, []),
        (3, [[(0, 1), (1, 1), (2, 0)], [(0, 0), (1, 1), (2, 1)]]),
        (1, [[(0, 1), (0, 1), (0, 1)], [(0, 0), (0, 0), (0, 0)]]),
    ]
    rng = random.Random(20260812)
    for n in [1, 4, 8, 12]:
        clauses: list[list[tuple[int, int]]] = []
        for _ in range(30):
            clauses.append([(rng.randrange(n), rng.randrange(2)) for _ in range(3)])
        cases.append((n, clauses))

    for i, case in enumerate(cases):
        write_case(out_dir, i, *case)


if __name__ == "__main__":
    main()
