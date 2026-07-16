#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


Clause = tuple[list[int], int]


def solve_naive(variable_count: int, clauses: list[Clause]) -> tuple[bool, list[int]]:
    models: list[int] = []
    for assignment in range(1 << variable_count):
        valid = True
        for negative, positive in clauses:
            satisfied = positive >= 0 and (assignment >> positive) & 1
            if not satisfied:
                satisfied = any(((assignment >> variable) & 1) == 0 for variable in negative)
            if not satisfied:
                valid = False
                break
        if valid:
            models.append(assignment)
    if not models:
        return False, []
    minimum = (1 << variable_count) - 1
    for model in models:
        minimum &= model
    return True, [(minimum >> variable) & 1 for variable in range(variable_count)]


def encode_case(variable_count: int, clauses: list[Clause]) -> list[str]:
    lines = [f"{variable_count} {len(clauses)}"]
    for negative, positive in clauses:
        tail = " ".join(map(str, negative))
        line = f"{positive} {len(negative)}"
        if tail:
            line += " " + tail
        lines.append(line)
    return lines


def answer_line(answer: tuple[bool, list[int]]) -> str:
    satisfiable, model = answer
    return " ".join(map(str, [int(satisfiable), *model]))


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[tuple[int, list[Clause]]] = [
        (0, []),
        (0, [([], -1)]),
        (1, [([], 0)]),
        (1, [([0], 0)]),
        (1, [([], 0), ([0], -1)]),
        (3, [([], 0), ([0], 1), ([1], 2)]),
        (3, [([0, 0], 1), ([], 0)]),
    ]
    rng = random.Random(20260716)
    for _ in range(600):
        variable_count = rng.randrange(9)
        clause_count = rng.randrange(18)
        clauses: list[Clause] = []
        for _ in range(clause_count):
            positive = -1
            if variable_count and rng.randrange(4):
                positive = rng.randrange(variable_count)
            negative_count = rng.randrange(variable_count + 4)
            negative = [
                rng.randrange(variable_count)
                for _ in range(negative_count)
            ] if variable_count else []
            clauses.append((negative, positive))
        cases.append((variable_count, clauses))

    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for variable_count, clauses in cases:
        input_lines.extend(encode_case(variable_count, clauses))
        output_lines.append(answer_line(solve_naive(variable_count, clauses)))
    (out_dir / "case_00.in").write_text(
        "\n".join(input_lines) + "\n", encoding="utf-8"
    )
    (out_dir / "case_00.out").write_text(
        "\n".join(output_lines) + "\n", encoding="utf-8"
    )

    variable_count = 100_000
    clauses: list[Clause] = [([], 0)]
    clauses.extend(([variable - 1], variable) for variable in range(1, variable_count))
    large_input = ["1", *encode_case(variable_count, clauses)]
    large_output = "1 " + " ".join(["1"] * variable_count)
    (out_dir / "case_01.in").write_text(
        "\n".join(large_input) + "\n", encoding="utf-8"
    )
    (out_dir / "case_01.out").write_text(
        large_output + "\n", encoding="utf-8"
    )


if __name__ == "__main__":
    main()
