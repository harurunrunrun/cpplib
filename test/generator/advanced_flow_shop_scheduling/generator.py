#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path


def makespan(matrix: list[list[int]], order: list[int]) -> int:
    machine_count = len(matrix[0]) if matrix else 0
    completion = [0] * machine_count
    for job in order:
        if not completion:
            continue
        completion[0] += matrix[job][0]
        for machine in range(1, machine_count):
            completion[machine] = max(completion[machine], completion[machine - 1])
            completion[machine] += matrix[job][machine]
    return completion[-1] if completion else 0


def johnson(first: list[float], second: list[float]) -> list[int]:
    early = [job for job in range(len(first)) if first[job] <= second[job]]
    late = [job for job in range(len(first)) if first[job] > second[job]]
    early.sort(key=lambda job: (first[job], job))
    late.sort(key=lambda job: (-second[job], job))
    return early + late


def cds(matrix: list[list[int]]) -> list[int]:
    size = len(matrix)
    machines = len(matrix[0]) if matrix else 0
    if size <= 1 or machines == 0:
        return list(range(size))
    if machines == 1:
        return sorted(range(size), key=lambda job: (matrix[job][0], job))
    best: list[int] | None = None
    best_value = 0
    for split in range(1, machines):
        first = [sum(matrix[job][:split]) / split for job in range(size)]
        second = [sum(matrix[job][machines - split:]) / split for job in range(size)]
        candidate = johnson(first, second)
        value = makespan(matrix, candidate)
        if best is None or value < best_value:
            best = candidate
            best_value = value
    return best or []


def palmer(matrix: list[list[int]]) -> list[int]:
    size = len(matrix)
    machines = len(matrix[0]) if matrix else 0
    if machines <= 1:
        return list(range(size))
    slope = [
        sum((machines - 1 - 2 * machine) * value for machine, value in enumerate(row))
        for row in matrix
    ]
    return sorted(range(size), key=lambda job: (-slope[job], job))


def gupta(matrix: list[list[int]]) -> list[int]:
    size = len(matrix)
    machines = len(matrix[0]) if matrix else 0
    if machines <= 1:
        return list(range(size))
    early = [row[0] < row[-1] for row in matrix]
    pair_minimum = [min((row[m] + row[m + 1]) / 2 for m in range(machines - 1)) for row in matrix]
    return sorted(range(size), key=lambda job: (0 if early[job] else 1, pair_minimum[job] if early[job] else -pair_minimum[job], job))


def write_cases(path: Path, cases: list[list[list[int]]]) -> None:
    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for matrix in cases:
        machines = len(matrix[0]) if matrix else 0
        input_lines.append(f"{len(matrix)} {machines}")
        input_lines.extend(" ".join(map(str, row)) for row in matrix)
        for order in (cds(matrix), palmer(matrix), gupta(matrix)):
            output_lines.append(" ".join(map(str, [makespan(matrix, order), len(order), *order])))
    path.with_suffix(".in").write_text("\n".join(input_lines) + "\n", encoding="utf-8")
    path.with_suffix(".out").write_text("\n".join(output_lines) + "\n", encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    deterministic = [[], [[]], [[5]], [[3], [1], [2]], [[2, 5], [7, 1], [4, 4]], [[5, 2, 7], [1, 8, 3], [6, 4, 2], [3, 5, 9]], [[0, 0, 0], [0, 2, 0], [3, 0, 1]]]
    write_cases(out_dir / "case_00", deterministic)
    rng = random.Random(2026071324)
    random_cases = []
    for _ in range(36):
        jobs = rng.randrange(1, 15)
        machines = rng.randrange(1, 8)
        random_cases.append([[rng.randrange(0, 100) for _ in range(machines)] for _ in range(jobs)])
    write_cases(out_dir / "case_01", random_cases)


if __name__ == "__main__":
    main()
