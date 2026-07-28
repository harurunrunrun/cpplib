#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


MOD = 998_244_353
Matrix = list[list[int]]


def inverse(series: Matrix) -> Matrix:
    rows = len(series)
    columns = len(series[0])
    result = [[0] * columns for _ in range(rows)]
    constant_inverse = pow(series[0][0], MOD - 2, MOD)
    result[0][0] = constant_inverse
    for total in range(1, rows + columns - 1):
        for row in range(max(0, total - columns + 1), min(rows, total + 1)):
            column = total - row
            value = 0
            for source_row in range(row + 1):
                for source_column in range(column + 1):
                    if source_row == 0 and source_column == 0:
                        continue
                    value += (
                        series[source_row][source_column]
                        * result[row - source_row][column - source_column]
                    )
            result[row][column] = -value * constant_inverse % MOD
    return result


def write_case(out_dir: Path, name: str, cases: list[Matrix]) -> None:
    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for series in cases:
        input_lines.append(f"{len(series)} {len(series[0])}")
        input_lines.extend(" ".join(map(str, row)) for row in series)
        output_lines.extend(
            " ".join(map(str, row)) for row in inverse(series)
        )
    (out_dir / f"{name}.in").write_text(
        "\n".join(input_lines) + "\n", encoding="utf-8"
    )
    (out_dir / f"{name}.out").write_text(
        "\n".join(output_lines) + "\n", encoding="utf-8"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", type=Path, required=True)
    parser.add_argument("--seed", type=int, default=20260728)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)
    write_case(
        args.out_dir,
        "fixed",
        [
            [[1]],
            [[2, 3, 5, 7]],
            [[3], [4], [5], [6]],
            [[1, 2, 3], [4, 5, 6]],
        ],
    )
    random_engine = random.Random(args.seed)
    cases: list[Matrix] = []
    for _ in range(30):
        rows = random_engine.randint(1, 10)
        columns = random_engine.randint(1, 10)
        series = [
            [random_engine.randrange(MOD) for _ in range(columns)]
            for _ in range(rows)
        ]
        series[0][0] = random_engine.randrange(1, MOD)
        cases.append(series)
    write_case(args.out_dir, "random", cases)


if __name__ == "__main__":
    main()
