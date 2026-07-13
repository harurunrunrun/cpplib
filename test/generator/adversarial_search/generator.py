#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def minimax(values: list[int], depth: int) -> int:
    current = values[:]
    maximizing = depth % 2 == 1
    while len(current) > 1:
        if maximizing:
            current = [max(current[index], current[index + 1]) for index in range(0, len(current), 2)]
        else:
            current = [min(current[index], current[index + 1]) for index in range(0, len(current), 2)]
        maximizing = not maximizing
    return current[0]


def write_case(output_directory: Path, index: int, depth: int, leaves: list[int]) -> None:
    answer = minimax(leaves, depth)
    stem = f"case_{index:02d}"
    (output_directory / f"{stem}.in").write_text(
        f"{depth}\n{' '.join(map(str, leaves))}\n", encoding="utf-8"
    )
    (output_directory / f"{stem}.out").write_text(
        " ".join([str(answer)] * 5) + "\n", encoding="utf-8"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    arguments = parser.parse_args()
    output_directory = Path(arguments.out_dir)
    output_directory.mkdir(parents=True, exist_ok=True)
    random_engine = random.Random(2718281828)

    cases: list[tuple[int, list[int]]] = [(0, [17]), (1, [-5, 8])]
    for depth in range(2, 11):
        leaves = [random_engine.randrange(-1_000_000, 1_000_001) for _ in range(1 << depth)]
        cases.append((depth, leaves))
    for index, (depth, leaves) in enumerate(cases):
        write_case(output_directory, index, depth, leaves)


if __name__ == "__main__":
    main()
