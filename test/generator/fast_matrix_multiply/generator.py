#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def multiply(
    left: list[int], rows: int, inner: int, right: list[int], columns: int
) -> list[int]:
    output = [0] * (rows * columns)
    for row in range(rows):
        for middle in range(inner):
            value = left[row * inner + middle]
            for column in range(columns):
                output[row * columns + column] += (
                    value * right[middle * columns + column]
                )
    return output


def values(rng: random.Random, size: int, bound: int = 7) -> list[int]:
    return [rng.randrange(-bound, bound + 1) for _ in range(size)]


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    arguments = parser.parse_args()

    out_dir = Path(arguments.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(20260722)

    cases: list[tuple[str, int, int, int, list[int], list[int]]] = []

    def add(operation: str, rows: int, inner: int, columns: int) -> None:
        left = values(rng, rows * inner)
        right = left if operation == "SAME" else values(rng, inner * columns)
        cases.append((operation, rows, inner, columns, left, right))

    add("FLAT", 0, 0, 0)
    add("FLAT", 0, 3, 4)
    add("FLAT", 3, 0, 4)
    add("FLAT", 3, 4, 0)
    add("FLAT", 1, 1, 1)
    add("FLAT", 2, 3, 4)
    add("FLAT", 3, 5, 7)
    add("FLAT", 63, 64, 65)
    add("FLAT", 65, 67, 66)
    add("FLAT", 127, 126, 125)
    add("FLAT", 2, 129, 3)
    add("FLAT", 129, 3, 127)
    add("SAME", 65, 65, 65)
    add("MATRIX", 0, 3, 4)
    add("MATRIX", 5, 0, 7)
    add("MATRIX", 67, 65, 66)

    for _ in range(30):
        rows = rng.randrange(1, 18)
        inner = rng.randrange(1, 18)
        columns = rng.randrange(1, 18)
        add(rng.choice(["FLAT", "MATRIX"]), rows, inner, columns)

    # Large enough to force two Strassen levels and catch accidental regressions
    # in the recursive path while keeping the complete verifier inexpensive.
    add("FLAT", 256, 256, 256)

    input_lines = [str(len(cases) + 1), "INVALID"]
    output_lines = ["OK"]
    for operation, rows, inner, columns, left, right in cases:
        if operation == "SAME":
            input_lines.append(f"SAME {rows}")
        else:
            input_lines.append(f"{operation} {rows} {inner} {columns}")
        if left:
            input_lines.append(" ".join(map(str, left)))
        if operation != "SAME" and right:
            input_lines.append(" ".join(map(str, right)))

        product = multiply(left, rows, inner, right, columns)
        output_lines.append(" ".join(map(str, [rows, columns, *product])))

    (out_dir / "case_00.in").write_text(
        "\n".join(input_lines) + "\n", encoding="utf-8"
    )
    (out_dir / "case_00.out").write_text(
        "\n".join(output_lines) + "\n", encoding="utf-8"
    )


if __name__ == "__main__":
    main()
