#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def row_minima(matrix: list[list[int]]) -> list[int]:
    return [min(range(len(row)), key=row.__getitem__) for row in matrix]


def write_case(out_dir: Path, index: int, matrix: list[list[int]]) -> None:
    rows = len(matrix)
    columns = len(matrix[0])
    input_lines = [f"{rows} {columns}"]
    input_lines.extend(" ".join(map(str, row)) for row in matrix)
    (out_dir / f"case_{index:02d}.in").write_text(
        "\n".join(input_lines) + "\n", encoding="utf-8"
    )
    (out_dir / f"case_{index:02d}.out").write_text(
        " ".join(map(str, row_minima(matrix))) + "\n", encoding="utf-8"
    )


def monge_matrix(rows: int, columns: int, rng: random.Random) -> list[list[int]]:
    row_bias = [rng.randrange(-10_000, 10_001) for _ in range(rows)]
    column_bias = [rng.randrange(-10_000, 10_001) for _ in range(columns)]
    scale = rng.randrange(0, 8)
    return [
        [
            scale * (row - column) ** 2 + row_bias[row] + column_bias[column]
            for column in range(columns)
        ]
        for row in range(rows)
    ]


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(2026071307)
    cases: list[list[list[int]]] = [
        [[0]],
        [[7, -2, -2, 5, 9]],
        [[3], [-4], [8], [8]],
        [[0] * 9 for _ in range(7)],
        [[(row - column) ** 2 for column in range(13)] for row in range(19)],
    ]
    for _ in range(25):
        rows = rng.randrange(1, 45)
        columns = rng.randrange(1, 45)
        cases.append(monge_matrix(rows, columns, rng))

    for index, matrix in enumerate(cases):
        write_case(args.out_dir, index, matrix)


if __name__ == "__main__":
    main()
