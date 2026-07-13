#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path


def makespan(matrix: list[list[int]], order: list[int]) -> int:
    if not matrix or not matrix[0]:
        return 0
    completion = [0] * len(matrix[0])
    for job in order:
        completion[0] += matrix[job][0]
        for machine in range(1, len(completion)):
            completion[machine] = max(completion[machine], completion[machine - 1])
            completion[machine] += matrix[job][machine]
    return completion[-1]


def neh_order(matrix: list[list[int]]) -> list[int]:
    jobs = sorted(range(len(matrix)), key=lambda job: -sum(matrix[job]))
    order: list[int] = []
    for job in jobs:
        candidates = [
            order[:position] + [job] + order[position:]
            for position in range(len(order) + 1)
        ]
        order = min(candidates, key=lambda candidate: makespan(matrix, candidate))
    return order


def johnson_order(matrix: list[list[int]]) -> list[int]:
    early = [job for job, row in enumerate(matrix) if row[0] <= row[1]]
    late = [job for job, row in enumerate(matrix) if row[0] > row[1]]
    early.sort(key=lambda job: matrix[job][0])
    late.sort(key=lambda job: -matrix[job][1])
    return early + late


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(2026071312)
    matrices: list[list[list[int]]] = [
        [],
        [[]],
        [[3, 7]],
        [[2, 9], [8, 3], [4, 4], [6, 5]],
        [[5, 2, 8], [7, 1, 3], [4, 9, 2], [6, 3, 7]],
        [[0, 0], [0, 0], [0, 0]],
    ]
    dimensions = [(0, 2), (1, 0), (1, 2), (4, 2), (4, 3), (3, 2)]
    for _ in range(34):
        jobs = rng.randrange(0, 16)
        machines = rng.randrange(1, 7)
        matrices.append([
            [rng.randrange(0, 5000) for _ in range(machines)]
            for _ in range(jobs)
        ])
        dimensions.append((jobs, machines))

    input_lines = [str(len(matrices))]
    output_lines: list[str] = []
    for matrix, (job_count, machine_count) in zip(matrices, dimensions):
        input_lines.append(f"{job_count} {machine_count}")
        input_lines.extend(" ".join(map(str, row)) for row in matrix)
        identity = list(range(job_count))
        neh = neh_order(matrix)
        row = [makespan(matrix, identity), makespan(matrix, neh), job_count, *neh]
        if machine_count == 2:
            johnson = johnson_order(matrix)
            row.extend([makespan(matrix, johnson), job_count, *johnson])
        else:
            row.extend([-1, 0])
        output_lines.append(" ".join(map(str, row)))

    (out_dir / "case_00.in").write_text(
        "\n".join(input_lines) + "\n", encoding="utf-8"
    )
    (out_dir / "case_00.out").write_text(
        "\n".join(output_lines) + "\n", encoding="utf-8"
    )


if __name__ == "__main__":
    main()
