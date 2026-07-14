#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


Clause = tuple[int, int, int, int]


def satisfiable(variable_count: int, clauses: list[Clause]) -> bool:
    for mask in range(1 << variable_count):
        valid = True
        for left, left_value, right, right_value in clauses:
            if ((mask >> left) & 1) != left_value and ((mask >> right) & 1) != right_value:
                valid = False
                break
        if valid:
            return True
    return False


def write_case(
    out_dir: Path,
    case_index: int,
    variable_count: int,
    clauses: list[Clause],
) -> None:
    stem = f"case_{case_index:03d}"
    lines = [f"{variable_count} {len(clauses)}"]
    lines.extend(" ".join(map(str, clause)) for clause in clauses)
    (out_dir / f"{stem}.in").write_text("\n".join(lines) + "\n", encoding="ascii")
    (out_dir / f"{stem}.out").write_text(
        f"{int(satisfiable(variable_count, clauses))}\n",
        encoding="ascii",
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    write_case(out_dir, 0, 0, [])
    write_case(out_dir, 1, 1, [(0, 1, 0, 1), (0, 0, 0, 0)])
    write_case(out_dir, 2, 2, [(0, 1, 1, 0), (0, 0, 1, 1)])

    rng = random.Random(2026071503)
    for case_index in range(3, 53):
        variable_count = rng.randrange(1, 11)
        clauses: list[Clause] = []
        for _ in range(rng.randrange(0, 45)):
            clauses.append(
                (
                    rng.randrange(variable_count),
                    rng.randrange(2),
                    rng.randrange(variable_count),
                    rng.randrange(2),
                )
            )
        write_case(out_dir, case_index, variable_count, clauses)


if __name__ == "__main__":
    main()
