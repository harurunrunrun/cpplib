#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def max_right(values: list[int], left: int, limit: int) -> int:
    total = 0
    right = left
    while right < len(values) and total + values[right] <= limit:
        total += values[right]
        right += 1
    return right


def min_left(values: list[int], right: int, limit: int) -> int:
    total = 0
    left = right
    while left > 0 and total + values[left - 1] <= limit:
        left -= 1
        total += values[left]
    return left


def write_case(out_dir: Path, index: int, n: int, operations: list[tuple]) -> None:
    values = [0] * n
    lines = [f"{n} {len(operations)}"]
    output: list[str] = []
    for operation in operations:
        lines.append(" ".join(map(str, operation)))
        kind = operation[0]
        if kind == "S":
            _, position, value = operation
            values[position] = value
        elif kind == "A":
            _, left, right, value = operation
            for position in range(left, right):
                values[position] += value
        elif kind == "P":
            _, position, value = operation
            values[position] += value
        elif kind == "Q":
            _, left, right = operation
            output.append(str(sum(values[left:right])))
        elif kind == "G":
            output.append(str(values[operation[1]]))
        elif kind == "R":
            output.append(str(max_right(values, operation[1], operation[2])))
        elif kind == "L":
            output.append(str(min_left(values, operation[1], operation[2])))
        else:
            output.append(str(sum(values)))
    stem = f"case_{index:02d}"
    (out_dir / f"{stem}.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (out_dir / f"{stem}.out").write_text("\n".join(output) + "\n", encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(0xD1A2E)
    for index, n in enumerate((1, 2, 17, 63, 64, 65, 257)):
        operations: list[tuple] = []
        for _ in range(240):
            choice = rng.randrange(8)
            position = rng.randrange(n)
            left = rng.randrange(n)
            right = rng.randrange(left + 1, n + 1)
            if choice == 0:
                operations.append(("S", position, rng.randrange(30)))
            elif choice == 1:
                operations.append(("A", left, right, rng.randrange(8)))
            elif choice == 2:
                operations.append(("P", position, rng.randrange(8)))
            elif choice == 3:
                operations.append(("Q", left, right))
            elif choice == 4:
                operations.append(("G", position))
            elif choice == 5:
                operations.append(("R", left, rng.randrange(100)))
            elif choice == 6:
                operations.append(("L", right, rng.randrange(100)))
            else:
                operations.append(("T",))
        write_case(out_dir, index, n, operations)


if __name__ == "__main__":
    main()
