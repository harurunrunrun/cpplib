#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


Operation = tuple[str, int, int, int]


def solve(initial: list[int], operations: list[Operation]) -> list[tuple[int, int]]:
    values = initial[:]
    answers: list[tuple[int, int]] = []
    for operation, left, right, operand in operations:
        if operation == "S":
            for index in range(left, right):
                values[index] = operand
        elif operation == "A":
            for index in range(left, right):
                values[index] += operand
        else:
            segment = values[left:right]
            answers.append((sum(segment), sum(value * value for value in segment)))
    return answers


def write_case(
    output_directory: Path,
    index: int,
    initial: list[int],
    operations: list[Operation],
    expected: list[tuple[int, int]] | None = None,
) -> None:
    name = f"case_{index:02d}"
    input_lines = [f"{len(initial)} {len(operations)}", " ".join(map(str, initial))]
    for operation, left, right, operand in operations:
        if operation == "Q":
            input_lines.append(f"Q {left} {right}")
        else:
            input_lines.append(f"{operation} {left} {right} {operand}")
    answers = expected if expected is not None else solve(initial, operations)
    (output_directory / f"{name}.in").write_text(
        "\n".join(input_lines) + "\n",
        encoding="utf-8",
    )
    (output_directory / f"{name}.out").write_text(
        "".join(f"{value_sum} {square_sum}\n" for value_sum, square_sum in answers),
        encoding="utf-8",
    )


def random_operations(
    random_number: random.Random,
    size: int,
    count: int,
) -> list[Operation]:
    result: list[Operation] = []
    for _ in range(count):
        operation_kind = random_number.randrange(5)
        if operation_kind <= 1:
            left = random_number.randrange(size + 1)
            right = random_number.randrange(left, size + 1)
            value = random_number.randrange(-10**6, 10**6 + 1)
            result.append(("S", left, right, value))
        elif operation_kind <= 3:
            left = random_number.randrange(size + 1)
            right = random_number.randrange(left, size + 1)
            delta = random_number.randrange(-10000, 10001)
            result.append(("A", left, right, delta))
        else:
            left = random_number.randrange(size)
            right = random_number.randrange(left + 1, size + 1)
            result.append(("Q", left, right, 0))
    return result


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    arguments = parser.parse_args()
    output_directory = Path(arguments.out_dir)
    output_directory.mkdir(parents=True, exist_ok=True)

    manual: list[Operation] = [
        ("Q", 0, 5, 0),
        ("A", 0, 3, -4),
        ("Q", 0, 3, 0),
        ("S", 1, 5, -7),
        ("Q", 0, 5, 0),
        ("A", 2, 5, 10),
        ("Q", 1, 4, 0),
        ("S", 2, 2, 123456789),
        ("A", 0, 0, -123456789),
        ("S", 0, 5, 0),
        ("Q", 0, 5, 0),
        ("A", 0, 5, -3),
        ("S", 2, 4, 5),
        ("A", 1, 4, -2),
        ("Q", 0, 5, 0),
    ]
    write_case(output_directory, 0, [-5, -1, 0, 2, 9], manual)

    random_number = random.Random(20260714)
    initial = [random_number.randrange(-10**6, 10**6 + 1) for _ in range(64)]
    write_case(
        output_directory,
        1,
        initial,
        random_operations(random_number, len(initial), 3000),
    )
    initial = [random_number.randrange(-10**6, 10**6 + 1) for _ in range(97)]
    write_case(
        output_directory,
        2,
        initial,
        random_operations(random_number, len(initial), 4000),
    )

    safe_limit = 2147483647
    boundary_operations: list[Operation] = [
        ("Q", 0, 2, 0),
        ("A", 0, 2, -1),
        ("Q", 0, 2, 0),
        ("S", 0, 1, -safe_limit),
        ("Q", 0, 2, 0),
        ("A", 0, 1, 1),
        ("S", 1, 2, safe_limit),
        ("Q", 0, 2, 0),
        ("S", 0, 2, 0),
        ("Q", 0, 2, 0),
    ]
    write_case(
        output_directory,
        3,
        [safe_limit, -safe_limit],
        boundary_operations,
    )

    single_limit = 3037000499
    single_operations: list[Operation] = [
        ("Q", 0, 1, 0),
        ("S", 0, 1, -single_limit),
        ("Q", 0, 1, 0),
        ("A", 0, 1, 1),
        ("Q", 0, 1, 0),
        ("S", 0, 1, single_limit),
        ("Q", 0, 1, 0),
    ]
    write_case(output_directory, 4, [single_limit], single_operations)

    size = 100000
    operation_count = 100000
    operations: list[Operation] = []
    expected: list[tuple[int, int]] = []
    current = 0
    for operation_index in range(operation_count):
        phase = operation_index % 4
        if phase == 0:
            current = (operation_index // 4) % 101 - 50
            operations.append(("S", 0, size, current))
        elif phase == 1:
            delta = 7 if operation_index % 8 == 1 else -11
            current += delta
            operations.append(("A", 0, size, delta))
        elif phase == 2:
            operations.append(("Q", 0, size, 0))
            expected.append((size * current, size * current * current))
        else:
            left = (operation_index * 1009) % size
            length = min(size - left, operation_index % 997 + 1)
            right = left + length
            operations.append(("Q", left, right, 0))
            expected.append((length * current, length * current * current))
    write_case(
        output_directory,
        5,
        [0] * size,
        operations,
        expected,
    )


if __name__ == "__main__":
    main()
