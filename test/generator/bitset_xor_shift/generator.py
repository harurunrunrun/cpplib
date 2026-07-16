#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


Operation = tuple[str, int, int]


def apply(bits: list[int], operation: Operation) -> None:
    kind, first, second = operation
    if kind in ("L", "R"):
        shift = first
        if shift >= len(bits):
            return
        old = bits.copy()
        if kind == "L":
            for index in range(shift, len(bits)):
                bits[index] ^= old[index - shift]
        else:
            for index in range(len(bits) - shift):
                bits[index] ^= old[index + shift]
    elif kind == "S":
        bits[first] = second
    elif kind == "C":
        bits[first] = 0
    elif kind == "F":
        bits[first] ^= 1
    elif kind == "A":
        bits[:] = [1] * len(bits)
    elif kind == "Z":
        bits[:] = [0] * len(bits)
    else:
        bits[:] = [value ^ 1 for value in bits]


def operation_line(operation: Operation) -> str:
    kind, first, second = operation
    if kind in ("L", "R", "C", "F"):
        return f"{kind} {first}"
    if kind == "S":
        return f"S {first} {second}"
    return kind


def write_case(
    out_dir: Path,
    index: int,
    initial: list[int],
    operations: list[Operation],
) -> None:
    bits = initial.copy()
    input_lines = [
        f"{len(bits)} {''.join(map(str, bits)) if bits else '-'}",
        str(len(operations)),
    ]
    output_lines: list[str] = []
    for operation in operations:
        input_lines.append(operation_line(operation))
        apply(bits, operation)
        output_lines.append(f"{sum(bits)} {''.join(map(str, bits))}")
    (out_dir / f"case_{index:02d}.in").write_text(
        "\n".join(input_lines) + "\n", encoding="utf-8"
    )
    (out_dir / f"case_{index:02d}.out").write_text(
        "\n".join(output_lines) + ("\n" if output_lines else ""),
        encoding="utf-8",
    )


def random_operations(
    generator: random.Random, size: int, count: int
) -> list[Operation]:
    result: list[Operation] = []
    for _ in range(count):
        kinds = ["L", "R", "A", "Z", "I"]
        if size:
            kinds += ["S", "C", "F"]
        kind = generator.choice(kinds)
        if kind in ("L", "R"):
            result.append((kind, generator.randrange(0, size + 130), 0))
        elif kind == "S":
            result.append((kind, generator.randrange(size), generator.randrange(2)))
        elif kind in ("C", "F"):
            result.append((kind, generator.randrange(size), 0))
        else:
            result.append((kind, 0, 0))
    return result


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[tuple[list[int], list[Operation]]] = [
        ([], [("L", 0, 0), ("R", 100, 0), ("A", 0, 0), ("I", 0, 0)]),
        ([0], [("S", 0, 1), ("L", 0, 0), ("F", 0, 0), ("R", 1, 0)]),
        (
            [int(index in {0, 1, 62, 63, 64, 65, 127, 128, 129}) for index in range(130)],
            [(kind, shift, 0) for shift in [0, 1, 63, 64, 65, 129, 130, 1000]
             for kind in ["L", "R"]],
        ),
    ]
    generator = random.Random(2026071614)
    for _ in range(30):
        size = generator.randrange(0, 500)
        initial = [generator.randrange(2) for _ in range(size)]
        cases.append((initial, random_operations(generator, size, 300)))
    size = 4096
    cases.append(
        (
            [generator.randrange(2) for _ in range(size)],
            random_operations(generator, size, 3000),
        )
    )
    for index, case in enumerate(cases):
        write_case(args.out_dir, index, *case)


if __name__ == "__main__":
    main()
