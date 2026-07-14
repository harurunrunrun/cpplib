#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def brute(grid: list[list[int]]) -> int:
    height = len(grid)
    width = len(grid[0])
    best = 0
    for row_mask in range(1 << height):
        for column_mask in range(1 << width):
            score = 0
            valid = True
            for row in range(height):
                for column in range(width):
                    row_selected = (row_mask >> row) & 1
                    column_selected = (column_mask >> column) & 1
                    value = grid[row][column]
                    if value < 0 and row_selected and column_selected:
                        valid = False
                        break
                    if row_selected or column_selected:
                        score += value
                if not valid:
                    break
            if valid:
                best = max(best, score)
    return best


def write_case(out_dir: Path, index: int, grid: list[list[int]]) -> None:
    lines = [f"{len(grid)} {len(grid[0])}"]
    lines.extend(" ".join(map(str, row)) for row in grid)
    name = f"case_{index:03d}"
    (out_dir / f"{name}.in").write_text("\n".join(lines) + "\n", encoding="ascii")
    (out_dir / f"{name}.out").write_text(f"{brute(grid)}\n", encoding="ascii")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    cases = [
        [[0]],
        [[7]],
        [[-9]],
        [[5, -4], [-3, 8]],
        [[1, 2, 3]],
        [[-1], [2], [-3]],
    ]
    rng = random.Random(259_259_07)
    for _ in range(74):
        height = rng.randint(1, 4)
        width = rng.randint(1, 4)
        cases.append(
            [[rng.randint(-30, 30) for _ in range(width)] for _ in range(height)]
        )

    for index, grid in enumerate(cases):
        write_case(out_dir, index, grid)


if __name__ == "__main__":
    main()
