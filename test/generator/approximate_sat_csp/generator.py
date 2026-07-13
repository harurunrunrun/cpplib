#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path

Clause = list[int]
Case = tuple[int, list[Clause], bool]


def satisfies(clauses: list[Clause], mask: int) -> bool:
    for clause in clauses:
        if not any(
            ((mask >> (abs(literal) - 1)) & 1) == (literal > 0)
            for literal in clause
        ):
            return False
    return True


def brute_force(variable_count: int, clauses: list[Clause]) -> bool:
    return any(satisfies(clauses, mask) for mask in range(1 << variable_count))


def make_cases() -> list[Case]:
    raw: list[tuple[int, list[Clause]]] = [
        (0, []),
        (0, [[]]),
        (1, [[1]]),
        (1, [[-1]]),
        (1, [[1], [-1]]),
        (2, [[1, -1, 1], [2, 2]]),
        (3, [[1, 2], [-1, 3], [-2, -3]]),
    ]
    rng = random.Random(20260713445)
    for _ in range(70):
        variable_count = rng.randrange(1, 11)
        clauses: list[Clause] = []
        for _ in range(rng.randrange(0, 36)):
            literal_count = rng.randrange(0, min(7, variable_count + 3))
            clause = []
            for _ in range(literal_count):
                variable = rng.randrange(1, variable_count + 1)
                clause.append(variable if rng.randrange(2) else -variable)
            clauses.append(clause)
        raw.append((variable_count, clauses))

    for _ in range(12):
        variable_count = 12
        planted = [rng.randrange(2) for _ in range(variable_count)]
        clauses = []
        for _ in range(55):
            variables = rng.sample(range(variable_count), 3)
            clause = []
            for variable in variables:
                positive = bool(rng.randrange(2))
                clause.append(variable + 1 if positive else -(variable + 1))
            if not any(
                planted[abs(literal) - 1] == (literal > 0) for literal in clause
            ):
                clause[0] = -clause[0]
            clauses.append(clause)
        raw.append((variable_count, clauses))

    return [
        (variable_count, clauses, brute_force(variable_count, clauses))
        for variable_count, clauses in raw
    ]


def write_case(path: Path, cases: list[Case]) -> None:
    lines = [str(len(cases))]
    satisfiable_count = 0
    for variable_count, clauses, oracle in cases:
        satisfiable_count += int(oracle)
        lines.append(f"{variable_count} {len(clauses)} {int(oracle)}")
        for clause in clauses:
            lines.append(str(len(clause)) + "".join(f" {literal}" for literal in clause))
    path.with_suffix(".in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    path.with_suffix(".out").write_text(
        f"OK {len(cases)} {satisfiable_count}\n", encoding="utf-8"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    cases = make_cases()
    midpoint = len(cases) // 2
    write_case(out_dir / "case_00", cases[:midpoint])
    write_case(out_dir / "case_01", cases[midpoint:])


if __name__ == "__main__":
    main()
