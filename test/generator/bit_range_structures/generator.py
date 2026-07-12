#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def write_case(out_dir: Path, index: int, mode: str, n: int, initial: list[int], operations: list[tuple]) -> None:
    values = initial[:]
    lines = [f"{mode} {n} {len(operations)}"]
    if mode != "OVERWRITE":
        lines.append(" ".join(map(str, initial)))
    output: list[str] = []
    for operation in operations:
        lines.append(" ".join(map(str, operation)))
        kind = operation[0]
        if kind == "A":
            _, left, right, value = operation
            values[left:right] = [value] * (right - left)
        elif kind == "F":
            _, left, right = operation
            values[left:right] = [value ^ 1 for value in values[left:right]]
        elif kind == "S":
            _, left, right = operation
            values[left:right] = [1] * (right - left)
        elif kind == "Q":
            _, left, right = operation
            answer = sum(values[left:right])
            output.append(f"{answer} {answer}")
        elif kind == "G":
            answer = values[operation[1]]
            output.append(f"{answer} {answer}")
        else:
            answer = sum(values)
            output.append(f"{answer} {answer}")
    stem = f"case_{index:02d}"
    (out_dir / f"{stem}.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (out_dir / f"{stem}.out").write_text("\n".join(output) + ("\n" if output else ""), encoding="utf-8")


def random_operations(rng: random.Random, mode: str, n: int, q: int) -> list[tuple]:
    operations: list[tuple] = []
    for _ in range(q):
        left = rng.randrange(n)
        right = rng.randrange(left + 1, n + 1)
        choice = rng.randrange(5)
        if mode == "ASSIGN":
            if choice < 2:
                operations.append(("A", left, right, rng.randrange(2)))
            elif choice < 4:
                operations.append(("Q", left, right))
            else:
                operations.append(("G", rng.randrange(n)))
        elif mode == "FLIP":
            if choice == 0:
                operations.append(("A", left, right, rng.randrange(2)))
            elif choice == 1:
                operations.append(("F", left, right))
            elif choice < 4:
                operations.append(("Q", left, right))
            else:
                operations.append(("G", rng.randrange(n)))
        else:
            if choice < 2:
                operations.append(("S", left, right))
            elif choice < 4:
                operations.append(("Q", left, right))
            else:
                operations.append(("T",))
    return operations


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(0xB1701)
    index = 0
    for mode in ("ASSIGN", "FLIP", "OVERWRITE"):
        for n in (1, 63, 64, 65, 255, 1025):
            initial = [rng.randrange(2) for _ in range(n)] if mode != "OVERWRITE" else [0] * n
            write_case(out_dir, index, mode, n, initial, random_operations(rng, mode, n, 180))
            index += 1


if __name__ == "__main__":
    main()
