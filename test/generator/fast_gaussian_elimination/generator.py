#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path

MOD = 998244353


def append_case(
    cases: list[tuple[list[list[int]], list[int], bool]],
    matrix: list[list[int]],
    rhs: list[int],
    check_all_overloads: bool = True,
) -> None:
    assert len(matrix) == len(rhs)
    assert not matrix or all(len(row) == len(matrix[0]) for row in matrix)
    cases.append((matrix, rhs, check_all_overloads))


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[tuple[list[list[int]], list[int], bool]] = []
    append_case(cases, [], [])
    append_case(cases, [[0]], [0])
    append_case(cases, [[0]], [1])
    append_case(cases, [[2]], [7])
    append_case(cases, [[1, 2, 3], [0, 1, 4]], [5, 6])
    append_case(cases, [[0, 1, 0], [1, 0, 1]], [7, 11])
    append_case(cases, [[1, 1, 0], [0, 0, 1]], [13, 17])
    append_case(cases, [[1, 0], [0, 1], [1, 1], [2, 3]], [4, 5, 9, 23])
    append_case(cases, [[1, 2], [2, 4], [3, 6]], [3, 6, 9])
    append_case(cases, [[1, 2], [2, 4], [3, 6]], [3, 7, 9])
    append_case(cases, [[], [], []], [0, 0, 0])

    source = random.Random(2026072247)
    for rows, columns in [
        (2, 7), (7, 2), (5, 9), (9, 5), (12, 12), (17, 23), (23, 17),
        (33, 48), (48, 33), (65, 65),
    ]:
        for _ in range(3):
            matrix = [
                [source.randrange(MOD) for _ in range(columns)]
                for _ in range(rows)
            ]
            rhs = [source.randrange(MOD) for _ in range(rows)]
            append_case(cases, matrix, rhs)

        rank_deficient = [
            [source.randrange(MOD) for _ in range(columns)]
            for _ in range(rows)
        ]
        if rows >= 2:
            rank_deficient[-1] = rank_deficient[0].copy()
        rhs = [source.randrange(MOD) for _ in range(rows)]
        if rows >= 2:
            rhs[-1] = rhs[0]
        append_case(cases, rank_deficient, rhs)
        if rows >= 2:
            inconsistent_rhs = rhs.copy()
            inconsistent_rhs[-1] = (inconsistent_rhs[-1] + 1) % MOD
            append_case(cases, rank_deficient, inconsistent_rhs)

    # Dense Cauchy matrix: nonsingular and large enough that 128 x 128
    # subproducts select the Strassen branch.
    size = 256
    left = list(range(1, size + 1))
    right = list(range(1000, 1000 + size))
    large = [
        [pow(x + y, MOD - 2, MOD) for y in right]
        for x in left
    ]
    rhs = [source.randrange(MOD) for _ in range(size)]
    append_case(cases, large, rhs, False)

    lines = [str(len(cases))]
    for matrix, rhs, check_all in cases:
        rows = len(matrix)
        columns = 0 if rows == 0 else len(matrix[0])
        lines.append(f"{rows} {columns} {int(check_all)}")
        lines.extend(" ".join(map(str, row)) for row in matrix)
        lines.append(" ".join(map(str, rhs)))
    (args.out_dir / "case_00.in").write_text(
        "\n".join(lines) + "\n", encoding="utf-8"
    )
    (args.out_dir / "case_00.out").write_text("OK\n", encoding="utf-8")


if __name__ == "__main__":
    main()
