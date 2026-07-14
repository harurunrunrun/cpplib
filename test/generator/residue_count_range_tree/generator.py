#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def write_case(
    out_dir: Path,
    index: int,
    modulus: int,
    initial: list[int],
    operations: list[tuple[int, ...]],
) -> None:
    values = initial[:]
    lines = [
        f"{modulus} {len(values)} {len(operations)}",
        " ".join(map(str, values)),
    ]
    output: list[str] = []
    for operation in operations:
        if operation[0] == 0:
            _, left, right = operation
            lines.append(f"I {left} {right}")
            for position in range(left, right):
                values[position] += 1
        else:
            _, left, right, residue = operation
            lines.append(f"C {left} {right} {residue}")
            target = residue % modulus
            output.append(
                str(sum(value % modulus == target for value in values[left:right]))
            )
    (out_dir / f"case_{index:02d}.in").write_text(
        "\n".join(lines) + "\n", encoding="utf-8"
    )
    (out_dir / f"case_{index:02d}.out").write_text(
        "\n".join(output) + "\n", encoding="utf-8"
    )


def random_case(rng: random.Random, modulus: int, size: int, count: int):
    initial = [rng.randint(-10**12, 10**12) for _ in range(size)]
    operations: list[tuple[int, ...]] = []
    for step in range(count):
        left = rng.randrange(size + 1)
        right = rng.randrange(size + 1)
        if left > right:
            left, right = right, left
        if step % 3:
            operations.append((0, left, right))
        else:
            operations.append(
                (1, left, right, rng.randint(-10**18, 10**18))
            )
    return initial, operations


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(2026071402)
    cases = []
    for modulus, size in ((1, 1), (2, 2), (3, 17), (5, 63), (7, 256)):
        cases.append((modulus, *random_case(rng, modulus, size, 260)))
    for index, case in enumerate(cases):
        write_case(out_dir, index, *case)


if __name__ == "__main__":
    main()
