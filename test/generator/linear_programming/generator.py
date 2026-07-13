#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import itertools
import random
from fractions import Fraction
from pathlib import Path


def solve_square(matrix: list[list[Fraction]], values: list[Fraction]) -> list[Fraction] | None:
    size = len(matrix)
    for column in range(size):
        pivot = next((row for row in range(column, size) if matrix[row][column]), None)
        if pivot is None:
            return None
        matrix[column], matrix[pivot] = matrix[pivot], matrix[column]
        values[column], values[pivot] = values[pivot], values[column]
        inverse = 1 / matrix[column][column]
        matrix[column] = [value * inverse for value in matrix[column]]
        values[column] *= inverse
        for row in range(size):
            if row == column:
                continue
            factor = matrix[row][column]
            matrix[row] = [
                matrix[row][entry] - factor * matrix[column][entry]
                for entry in range(size)
            ]
            values[row] -= factor * values[column]
    return values


def bounded_optimum(
    variable_count: int,
    rows: list[list[int]],
    bounds: list[int],
    objective: list[int],
) -> Fraction:
    all_rows = [[Fraction(value) for value in row] for row in rows]
    all_bounds = [Fraction(value) for value in bounds]
    for variable in range(variable_count):
        row = [Fraction(0)] * variable_count
        row[variable] = -1
        all_rows.append(row)
        all_bounds.append(Fraction(0))

    answer: Fraction | None = None
    for selected in itertools.combinations(range(len(all_rows)), variable_count):
        solution = solve_square(
            [all_rows[index].copy() for index in selected],
            [all_bounds[index] for index in selected],
        )
        if solution is None:
            continue
        if any(
            sum(all_rows[index][variable] * solution[variable]
                for variable in range(variable_count)) > all_bounds[index]
            for index in range(len(all_rows))
        ):
            continue
        value = sum(
            Fraction(objective[variable]) * solution[variable]
            for variable in range(variable_count)
        )
        if answer is None or value > answer:
            answer = value
    if answer is None:
        raise AssertionError("generated bounded problem is infeasible")
    return answer


def round_scaled(value: Fraction) -> int:
    scaled = value * 1_000_000
    sign = -1 if scaled < 0 else 1
    numerator = abs(scaled.numerator)
    quotient, remainder = divmod(numerator, scaled.denominator)
    if 2 * remainder >= scaled.denominator:
        quotient += 1
    return sign * quotient


def write_case(
    out_dir: Path,
    index: int,
    variable_count: int,
    rows: list[list[int]],
    bounds: list[int],
    objective: list[int],
    answer: list[str],
) -> None:
    lines = [f"{len(rows)} {variable_count}"]
    lines.extend(
        " ".join(map(str, row + [bound]))
        for row, bound in zip(rows, bounds)
    )
    lines.append(" ".join(map(str, objective)))
    (out_dir / f"case_{index:02d}.in").write_text(
        "\n".join(lines) + "\n", encoding="utf-8"
    )
    (out_dir / f"case_{index:02d}.out").write_text(
        "\n".join(answer) + "\n", encoding="utf-8"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)

    write_case(
        args.out_dir,
        0,
        2,
        [[1, 1], [1, 0], [0, 1]],
        [4, 2, 3],
        [3, 2],
        ["OPTIMAL", "10000000"],
    )
    write_case(
        args.out_dir, 1, 1, [[-1], [1]], [-2, 1], [1], ["INFEASIBLE"]
    )
    write_case(args.out_dir, 2, 1, [], [], [1], ["UNBOUNDED"])
    write_case(
        args.out_dir, 3, 2, [[0, 1]], [5], [1, 0], ["UNBOUNDED"]
    )

    rng = random.Random(2026071319)
    case_index = 4
    for _ in range(36):
        variable_count = rng.randint(1, 4)
        lower = [rng.randint(0, 5) for _ in range(variable_count)]
        upper = [lower[index] + rng.randint(1, 8) for index in range(variable_count)]
        witness = [rng.randint(lower[index], upper[index]) for index in range(variable_count)]
        rows: list[list[int]] = []
        bounds: list[int] = []
        for variable in range(variable_count):
            upper_row = [0] * variable_count
            upper_row[variable] = 1
            rows.append(upper_row)
            bounds.append(upper[variable])
            lower_row = [0] * variable_count
            lower_row[variable] = -1
            rows.append(lower_row)
            bounds.append(-lower[variable])
        for _ in range(rng.randint(0, 12 - len(rows))):
            row = [rng.randint(-4, 4) for _ in range(variable_count)]
            if all(value == 0 for value in row):
                row[rng.randrange(variable_count)] = 1
            bound = sum(row[index] * witness[index] for index in range(variable_count))
            bound += rng.randint(0, 8)
            rows.append(row)
            bounds.append(bound)
        objective = [rng.randint(-7, 7) for _ in range(variable_count)]
        value = bounded_optimum(variable_count, rows, bounds, objective)
        write_case(
            args.out_dir,
            case_index,
            variable_count,
            rows,
            bounds,
            objective,
            ["OPTIMAL", str(round_scaled(value))],
        )
        case_index += 1


if __name__ == "__main__":
    main()
