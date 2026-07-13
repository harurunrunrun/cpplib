#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import math
import random
from pathlib import Path


def schedule(matrix: list[list[int]], rule: str) -> tuple[int, list[int]]:
    jobs = len(matrix)
    machines = len(matrix[0]) if matrix else 0
    load = [0] * machines
    assignment = [0] * jobs
    done = [False] * jobs
    for _ in range(jobs):
        choices = []
        for job in range(jobs):
            if done[job]:
                continue
            completion = sorted((load[m] + matrix[job][m], m) for m in range(machines))
            best, machine = completion[0]
            sufferage = completion[1][0] - best if machines > 1 else math.inf
            choices.append((job, best, machine, sufferage))
        if rule == "min":
            job, best, machine, _ = min(choices, key=lambda value: (value[1], value[0]))
        elif rule == "max":
            job, best, machine, _ = min(choices, key=lambda value: (-value[1], value[0]))
        else:
            job, best, machine, _ = min(choices, key=lambda value: (-value[3], value[1], value[0]))
        done[job] = True
        assignment[job] = machine
        load[machine] = best
    return max(load, default=0), assignment


def write_cases(path: Path, cases: list[list[list[int]]]) -> None:
    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for matrix in cases:
        machines = len(matrix[0]) if matrix else 0
        input_lines.append(f"{len(matrix)} {machines}")
        input_lines.extend(" ".join(map(str, row)) for row in matrix)
        for rule in ("min", "max", "sufferage"):
            makespan, assignment = schedule(matrix, rule)
            output_lines.append(" ".join(map(str, [makespan, len(assignment), *assignment])))
    path.with_suffix(".in").write_text("\n".join(input_lines) + "\n", encoding="utf-8")
    path.with_suffix(".out").write_text("\n".join(output_lines) + "\n", encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    deterministic = [[], [[5]], [[5, 1, 9]], [[2, 8], [4, 3], [7, 1]], [[0, 0], [0, 1], [1, 0], [0, 0]], [[9, 2, 7], [6, 4, 3], [5, 8, 1], [2, 9, 6]]]
    write_cases(out_dir / "case_00", deterministic)
    rng = random.Random(2026071325)
    random_cases = []
    for _ in range(42):
        jobs = rng.randrange(1, 18)
        machines = rng.randrange(1, 8)
        random_cases.append([[rng.randrange(0, 150) for _ in range(machines)] for _ in range(jobs)])
    write_cases(out_dir / "case_01", random_cases)


if __name__ == "__main__":
    main()
