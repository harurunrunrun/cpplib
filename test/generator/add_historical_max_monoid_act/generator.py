#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


Operation = tuple[str, int, int, int]


def solve(initial: list[int], operations: list[Operation]) -> list[tuple[int, int]]:
    current = initial[:]
    historical = initial[:]
    answers: list[tuple[int, int]] = []
    for operation_type, left, right, delta in operations:
        if operation_type == "A":
            for index in range(left, right):
                current[index] += delta
                historical[index] = max(historical[index], current[index])
        else:
            answers.append((max(current[left:right]), max(historical[left:right])))
    return answers


def write_case(
    output_directory: Path,
    index: int,
    type_name: str,
    initial: list[int],
    operations: list[Operation],
    expected: list[tuple[int, int]] | None = None,
) -> None:
    name = f"case_{index:02d}"
    input_lines = [type_name, f"{len(initial)} {len(operations)}", " ".join(map(str, initial))]
    for operation_type, left, right, delta in operations:
        if operation_type == "A":
            input_lines.append(f"A {left} {right} {delta}")
        else:
            input_lines.append(f"Q {left} {right}")
    answers = expected if expected is not None else solve(initial, operations)
    (output_directory / f"{name}.in").write_text(
        "\n".join(input_lines) + "\n",
        encoding="utf-8",
    )
    (output_directory / f"{name}.out").write_text(
        "".join(f"{current} {historical}\n" for current, historical in answers),
        encoding="utf-8",
    )


def random_operations(
    rng: random.Random,
    size: int,
    count: int,
    delta_limit: int,
) -> list[Operation]:
    result: list[Operation] = []
    for _ in range(count):
        left = rng.randrange(size)
        right = rng.randrange(left + 1, size + 1)
        if rng.randrange(3) != 0:
            result.append(("A", left, right, rng.randrange(-delta_limit, delta_limit + 1)))
        else:
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
        ("A", 0, 3, 7),
        ("Q", 0, 5, 0),
        ("A", 1, 5, -12),
        ("Q", 0, 5, 0),
        ("A", 2, 2, 999999),
        ("A", 2, 5, 20),
        ("Q", 1, 4, 0),
        ("A", 0, 5, -100),
        ("Q", 0, 5, 0),
    ]
    write_case(output_directory, 0, "I64", [0, -3, 5, -8, 2], manual)

    rng = random.Random(20260714)
    initial32 = [rng.randrange(-1000000, 1000001) for _ in range(64)]
    write_case(
        output_directory,
        1,
        "I32",
        initial32,
        random_operations(rng, len(initial32), 2000, 10000),
    )
    initial64 = [rng.randrange(-10**12, 10**12 + 1) for _ in range(77)]
    write_case(
        output_directory,
        2,
        "I64",
        initial64,
        random_operations(rng, len(initial64), 2500, 10**7),
    )

    int32_min = -(1 << 31)
    int32_max = (1 << 31) - 1
    boundary32 = [int32_min + 1000, -1, 0, int32_max - 1000]
    operations32: list[Operation] = [
        ("Q", 0, 4, 0),
        ("A", 0, 1, -400),
        ("A", 3, 4, 400),
        ("Q", 0, 4, 0),
        ("A", 1, 3, int32_max - 1000),
        ("Q", 1, 3, 0),
        ("A", 1, 3, -(int32_max - 1000)),
        ("Q", 0, 4, 0),
    ]
    write_case(output_directory, 3, "I32", boundary32, operations32)

    int64_min = -(1 << 63)
    int64_max = (1 << 63) - 1
    boundary64 = [int64_min + 10000, -9, 0, int64_max - 10000]
    operations64: list[Operation] = [
        ("Q", 0, 4, 0),
        ("A", 0, 1, -4000),
        ("A", 3, 4, 4000),
        ("Q", 0, 4, 0),
        ("A", 1, 3, 10**15),
        ("Q", 1, 3, 0),
        ("A", 1, 3, -(10**15)),
        ("Q", 0, 4, 0),
    ]
    write_case(output_directory, 4, "I64", boundary64, operations64)

    size = 100000
    operation_count = 100000
    operations: list[Operation] = []
    expected: list[tuple[int, int]] = []
    current = 0
    historical = 0
    for index in range(operation_count):
        if index % 2 == 0:
            delta = 17 if index % 4 == 0 else -11
            operations.append(("A", 0, size, delta))
            current += delta
            historical = max(historical, current)
        else:
            operations.append(("Q", 0, size, 0))
            expected.append((current, historical))
    write_case(output_directory, 5, "I64", [0] * size, operations, expected)


if __name__ == "__main__":
    main()
