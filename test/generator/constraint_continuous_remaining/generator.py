#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path

from oracle import solve_diagonal


def write_case(
    output: Path,
    index: int,
    diagonal: list[int],
    solution: list[int],
) -> None:
    right_hand_side = [
        coefficient * value for coefficient, value in zip(diagonal, solution)
    ]
    answer = solve_diagonal(diagonal, right_hand_side)
    stem = f"case_{index:02d}"
    (output / f"{stem}.in").write_text(
        f"{len(diagonal)}\n"
        f"{' '.join(map(str, diagonal))}\n"
        f"{' '.join(map(str, right_hand_side))}\n",
        encoding="utf-8",
    )
    (output / f"{stem}.out").write_text(
        f"{' '.join(map(str, answer))}\n",
        encoding="utf-8",
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    arguments = parser.parse_args()
    output = Path(arguments.out_dir)
    output.mkdir(parents=True, exist_ok=True)

    cases = [
        ([2], [3]),
        ([1, 4], [-5, 7]),
        ([3, 5, 7], [0, -2, 9]),
    ]
    random_engine = random.Random(20260729)
    for size in (8, 32, 100):
        diagonal = [random_engine.randrange(1, 50) for _ in range(size)]
        solution = [random_engine.randrange(-100, 101) for _ in range(size)]
        cases.append((diagonal, solution))
    for index, (diagonal, solution) in enumerate(cases):
        write_case(output, index, diagonal, solution)


if __name__ == "__main__":
    main()
