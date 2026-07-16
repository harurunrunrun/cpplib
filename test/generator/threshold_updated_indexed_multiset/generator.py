#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path

MINIMUM = -(1 << 63)
MAXIMUM = (1 << 63) - 1


def simulate(
    values: list[int],
    operations: list[tuple[str, int]],
) -> list[str]:
    current = values[:]
    output: list[str] = []
    for operation, argument in operations:
        if operation == "I":
            if current[argument] == MAXIMUM:
                output.append("overflow")
            else:
                current[argument] += 1
                output.append("ok")
        elif operation == "C":
            output.append(str(sum(value >= argument for value in current)))
        elif operation == "D":
            affected = [i for i, value in enumerate(current) if value >= argument]
            if any(current[i] == MINIMUM for i in affected):
                output.append("overflow")
            else:
                for i in affected:
                    current[i] -= 1
                output.append("ok")
        else:
            output.append(str(current[argument]))
    return output


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[tuple[list[int], list[tuple[str, int]]]] = [
        ([], [("C", 0), ("D", 0), ("C", MINIMUM)]),
        ([MAXIMUM], [("I", 0), ("G", 0), ("D", MAXIMUM), ("G", 0)]),
        ([MINIMUM], [("D", MINIMUM), ("G", 0), ("I", 0), ("G", 0)]),
        ([2, 2, 5], [
            ("I", 0), ("I", 1), ("C", 3), ("D", 3),
            ("C", 2), ("G", 0), ("G", 1), ("G", 2),
        ]),
    ]
    random_source = random.Random(0x7A12E5)
    for size in range(1, 65):
        values = [random_source.randint(-100, 100) for _ in range(size)]
        operations: list[tuple[str, int]] = []
        for _ in range(100):
            operation = random_source.choice(("I", "C", "D", "G"))
            argument = (
                random_source.randrange(size)
                if operation in ("I", "G")
                else random_source.randint(-110, 110)
            )
            operations.append((operation, argument))
        cases.append((values, operations))

    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for values, operations in cases:
        input_lines.append(f"{len(values)} {len(operations)}")
        input_lines.append(" ".join(map(str, values)))
        input_lines.extend(f"{operation} {argument}" for operation, argument in operations)
        output_lines.extend(simulate(values, operations))

    (out_dir / "case_00.in").write_text(
        "\n".join(input_lines) + "\n",
        encoding="ascii",
    )
    (out_dir / "case_00.out").write_text(
        "\n".join(output_lines) + "\n",
        encoding="ascii",
    )


if __name__ == "__main__":
    main()
