#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path

MOD = 998244353


def inv(value: int) -> int:
    return pow(value, MOD - 2, MOD)


def rank_mod(matrix: list[list[int]]) -> int:
    a = [[value % MOD for value in row] for row in matrix]
    rows = len(a)
    cols = len(a[0]) if rows else 0
    rank = 0
    for column in range(cols):
        pivot = next((i for i in range(rank, rows) if a[i][column]), None)
        if pivot is None:
            continue
        a[rank], a[pivot] = a[pivot], a[rank]
        inverse = inv(a[rank][column])
        for i in range(rank + 1, rows):
            if not a[i][column]:
                continue
            factor = a[i][column] * inverse % MOD
            for j in range(column, cols):
                a[i][j] = (a[i][j] - factor * a[rank][j]) % MOD
        rank += 1
    return rank


def determinant(matrix: list[list[int]]) -> int:
    a = [[value % MOD for value in row] for row in matrix]
    n = len(a)
    result = 1
    for column in range(n):
        pivot = next((i for i in range(column, n) if a[i][column]), None)
        if pivot is None:
            return 0
        if pivot != column:
            a[column], a[pivot] = a[pivot], a[column]
            result = -result
        value = a[column][column]
        result = result * value % MOD
        inverse = inv(value)
        for i in range(column + 1, n):
            factor = a[i][column] * inverse % MOD
            for j in range(column, n):
                a[i][j] = (a[i][j] - factor * a[column][j]) % MOD
    return result % MOD


def solve_mod(matrix: list[list[int]], rhs: list[int]) -> tuple[list[int], list[list[int]]] | None:
    rows = len(matrix)
    cols = len(matrix[0]) if rows else 0
    a = [[*(value % MOD for value in row), rhs[i] % MOD] for i, row in enumerate(matrix)]
    pivots: list[int] = []
    rank = 0
    for column in range(cols):
        pivot = next((i for i in range(rank, rows) if a[i][column]), None)
        if pivot is None:
            continue
        a[rank], a[pivot] = a[pivot], a[rank]
        inverse = inv(a[rank][column])
        for j in range(column, cols + 1):
            a[rank][j] = a[rank][j] * inverse % MOD
        for i in range(rows):
            if i == rank or not a[i][column]:
                continue
            factor = a[i][column]
            for j in range(column, cols + 1):
                a[i][j] = (a[i][j] - factor * a[rank][j]) % MOD
        pivots.append(column)
        rank += 1
    if any(a[i][cols] for i in range(rank, rows)):
        return None
    particular = [0] * cols
    is_pivot = [False] * cols
    for i, column in enumerate(pivots):
        is_pivot[column] = True
        particular[column] = a[i][cols]
    basis: list[list[int]] = []
    for free in range(cols):
        if is_pivot[free]:
            continue
        vector = [0] * cols
        vector[free] = 1
        for i, column in enumerate(pivots):
            vector[column] = -a[i][free] % MOD
        basis.append(vector)
    return particular, basis


def inverse_mod(matrix: list[list[int]]) -> list[list[int]] | None:
    n = len(matrix)
    augmented = [
        [*(value % MOD for value in matrix[i]), *(int(i == j) for j in range(n))]
        for i in range(n)
    ]
    for column in range(n):
        pivot = next((i for i in range(column, n) if augmented[i][column]), None)
        if pivot is None:
            return None
        augmented[column], augmented[pivot] = augmented[pivot], augmented[column]
        inverse = inv(augmented[column][column])
        for j in range(column, 2 * n):
            augmented[column][j] = augmented[column][j] * inverse % MOD
        for i in range(n):
            if i == column or not augmented[i][column]:
                continue
            factor = augmented[i][column]
            for j in range(column, 2 * n):
                augmented[i][j] = (augmented[i][j] - factor * augmented[column][j]) % MOD
    return [row[n:] for row in augmented]


def gf2_solve(rows: list[str], rhs: str) -> tuple[str, list[str]] | None:
    row_count = len(rows)
    cols = len(rows[0]) if rows else 0
    a = [[int(char) for char in row] + [int(rhs[i])] for i, row in enumerate(rows)]
    pivots: list[int] = []
    rank = 0
    for column in range(cols):
        pivot = next((i for i in range(rank, row_count) if a[i][column]), None)
        if pivot is None:
            continue
        a[rank], a[pivot] = a[pivot], a[rank]
        for i in range(row_count):
            if i != rank and a[i][column]:
                a[i] = [x ^ y for x, y in zip(a[i], a[rank])]
        pivots.append(column)
        rank += 1
    if any(a[i][cols] for i in range(rank, row_count)):
        return None
    particular = [0] * cols
    is_pivot = [False] * cols
    for i, column in enumerate(pivots):
        is_pivot[column] = True
        particular[column] = a[i][cols]
    basis: list[str] = []
    for free in range(cols):
        if is_pivot[free]:
            continue
        vector = [0] * cols
        vector[free] = 1
        for i, column in enumerate(pivots):
            vector[column] = a[i][free]
        basis.append("".join(map(str, vector)))
    return "".join(map(str, particular)), basis


def gf2_rank(rows: list[str]) -> int:
    if not rows:
        return 0
    solution = gf2_solve(rows, "0" * len(rows))
    assert solution is not None
    return len(rows[0]) - len(solution[1])


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(20260902)
    operations: list[tuple[str, list[list[int]], list[int] | None]] = []
    operations.extend([
        ("RANK", [[1, 2, 3], [2, 4, 6], [1, 1, 1]], None),
        ("DET", [[2, 1], [5, 3]], None),
        ("INV", [[2, 1], [5, 3]], None),
        ("INV", [[1, 2], [2, 4]], None),
        ("SOLVE", [[1, 2, 3], [2, 4, 6]], [4, 8]),
        ("SOLVE", [[1, 2, 3], [2, 4, 6]], [4, 9]),
    ])
    for _ in range(35):
        rows = rng.randrange(1, 9)
        cols = rng.randrange(1, 9)
        matrix = [[rng.randrange(-8, 9) for _ in range(cols)] for _ in range(rows)]
        operations.append(("RANK", matrix, None))
        operations.append(("SOLVE", matrix, [rng.randrange(-8, 9) for _ in range(rows)]))
    for _ in range(18):
        n = rng.randrange(1, 9)
        matrix = [[rng.randrange(-8, 9) for _ in range(n)] for _ in range(n)]
        operations.append(("DET", matrix, None))
        operations.append(("INV", matrix, None))

    gf2_operations: list[tuple[str, list[str], str | None]] = [
        ("G2RANK", ["101", "011", "110"], None),
        ("G2SOLVE", ["101", "011", "110"], "110"),
        ("G2SOLVE", ["00"], "1"),
    ]
    for _ in range(45):
        rows = rng.randrange(1, 13)
        cols = rng.randrange(1, 13)
        matrix = ["".join(str(rng.randrange(2)) for _ in range(cols)) for _ in range(rows)]
        gf2_operations.append(("G2RANK", matrix, None))
        gf2_operations.append(("G2SOLVE", matrix, "".join(str(rng.randrange(2)) for _ in range(rows))))

    input_lines = [str(len(operations) + len(gf2_operations))]
    output_lines: list[str] = []
    for operation, matrix, rhs in operations:
        rows = len(matrix)
        cols = len(matrix[0])
        input_lines.append(f"{operation} {rows} {cols}")
        input_lines.extend(" ".join(map(str, row)) for row in matrix)
        if rhs is not None:
            input_lines.append(" ".join(map(str, rhs)))
        if operation == "RANK":
            output_lines.append(str(rank_mod(matrix)))
        elif operation == "DET":
            output_lines.append(str(determinant(matrix)))
        elif operation == "INV":
            inverse = inverse_mod(matrix)
            if inverse is None:
                output_lines.append("-1")
            else:
                output_lines.extend(" ".join(map(str, row)) for row in inverse)
        else:
            solution = solve_mod(matrix, rhs or [])
            if solution is None:
                output_lines.append("-1")
            else:
                particular, basis = solution
                output_lines.append(str(len(basis)))
                output_lines.append(" ".join(map(str, particular)))
                output_lines.extend(" ".join(map(str, vector)) for vector in basis)

    for operation, matrix, rhs in gf2_operations:
        rows = len(matrix)
        cols = len(matrix[0])
        input_lines.append(f"{operation} {rows} {cols}")
        input_lines.extend(matrix)
        if rhs is not None:
            input_lines.append(rhs)
        if operation == "G2RANK":
            output_lines.append(str(gf2_rank(matrix)))
        else:
            solution = gf2_solve(matrix, rhs or "")
            if solution is None:
                output_lines.append("-1")
            else:
                particular, basis = solution
                output_lines.append(str(len(basis)))
                output_lines.append(particular)
                output_lines.extend(basis)

    (out_dir / "case_00.in").write_text("\n".join(input_lines) + "\n", encoding="utf-8")
    (out_dir / "case_00.out").write_text("\n".join(output_lines) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
