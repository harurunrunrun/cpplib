#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path


def solve(matrix):
    height = len(matrix)
    width = len(matrix[0])
    best = 0
    for row_mask in range(1 << height):
        for column_mask in range(1 << width):
            score = 0
            valid = True
            for row in range(height):
                row_selected = (row_mask >> row) & 1
                for column in range(width):
                    column_selected = (column_mask >> column) & 1
                    value = matrix[row][column]
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


def write_case(out_dir, index, matrix):
    lines = [f"{len(matrix)} {len(matrix[0])}"] + [" ".join(map(str, row)) for row in matrix]
    (out_dir / f"case_{index:02d}.in").write_text("\n".join(lines) + "\n")
    (out_dir / f"case_{index:02d}.out").write_text(f"{solve(matrix)}\n")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    cases = [[[0]], [[7]], [[-9]], [[5, -4], [-3, 8]],
             [[1, 2, 3]], [[-1], [2], [-3]]]
    rng = random.Random(25907)
    for height, width in [(2, 3), (3, 2), (3, 3), (4, 2)]:
        cases.append([[rng.randint(-8, 8) for _ in range(width)] for _ in range(height)])
    for index, matrix in enumerate(cases):
        write_case(out_dir, index, matrix)


if __name__ == "__main__":
    main()
