#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def minimax(values: list[int], maximizing: bool = True) -> int:
    current = values
    depth = len(values).bit_length() - 1
    turn = maximizing if depth % 2 == 1 else not maximizing
    while len(current) > 1:
        choose = max if turn else min
        current = [choose(current[index], current[index + 1])
                   for index in range(0, len(current), 2)]
        turn = not turn
    return current[0]


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    arguments = parser.parse_args()
    output = Path(arguments.out_dir)
    output.mkdir(parents=True, exist_ok=True)
    engine = random.Random(20260729)
    cases = [
        [42],
        [-5, 8],
        [3, 4, 5, 6],
        [9, -2, 7, 1, 0, 30, 4, 5],
    ]
    for exponent in (4, 7, 10):
        cases.append([
            engine.randrange(-1_000_000, 1_000_001)
            for _ in range(1 << exponent)
        ])
    for index, values in enumerate(cases):
        stem = f"case_{index:02d}"
        (output / f"{stem}.in").write_text(
            f"{len(values)}\n{' '.join(map(str, values))}\n",
            encoding="utf-8",
        )
        (output / f"{stem}.out").write_text(
            f"{minimax(values)}\n", encoding="utf-8"
        )


if __name__ == "__main__":
    main()
