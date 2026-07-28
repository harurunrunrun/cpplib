#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


Matrix = list[list[int]]


def answer(matrix: Matrix) -> str:
    return " ".join(
        str(min(range(len(row)), key=row.__getitem__))
        for row in matrix
    )


def write_case(out_dir: Path, name: str, matrices: list[Matrix]) -> None:
    lines = [str(len(matrices))]
    for matrix in matrices:
        lines.append(f"{len(matrix)} {len(matrix[0])}")
        lines.extend(" ".join(map(str, row)) for row in matrix)
    (out_dir / f"{name}.in").write_text(
        "\n".join(lines) + "\n", encoding="utf-8"
    )
    (out_dir / f"{name}.out").write_text(
        "\n".join(answer(matrix) for matrix in matrices) + "\n",
        encoding="utf-8",
    )


def generated_matrix(
    random_engine: random.Random, rows: int, columns: int
) -> Matrix:
    centers: list[int] = []
    current = random_engine.randrange(columns)
    for _ in range(rows):
        current = random_engine.randrange(current, columns)
        centers.append(current)
    return [
        [
            (column - centers[row]) ** 2 * 1000
            + random_engine.randrange(0, 100)
            for column in range(columns)
        ]
        for row in range(rows)
    ]


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", type=Path, required=True)
    parser.add_argument("--seed", type=int, default=20260728)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)
    random_engine = random.Random(args.seed)

    write_case(
        args.out_dir,
        "fixed",
        [
            [[7]],
            [[0, 1, 4, 9], [4, 1, 0, 1], [9, 4, 1, 0]],
            [[0, 0, 1], [1, 0, 0]],
        ],
    )
    write_case(
        args.out_dir,
        "random",
        [
            generated_matrix(
                random_engine,
                random_engine.randint(1, 80),
                random_engine.randint(1, 90),
            )
            for _ in range(40)
        ],
    )


if __name__ == "__main__":
    main()
