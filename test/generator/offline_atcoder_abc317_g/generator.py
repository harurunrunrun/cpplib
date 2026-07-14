#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path


def write_case(out_dir, index, matrix, answer):
    n = len(matrix)
    m = len(matrix[0])
    input_lines = [f"{n} {m}"] + [" ".join(map(str, row)) for row in matrix]
    output_lines = ["Yes"] + [" ".join(map(str, row)) for row in answer]
    (out_dir / f"case_{index:02d}.in").write_text("\n".join(input_lines) + "\n")
    (out_dir / f"case_{index:02d}.out").write_text("\n".join(output_lines) + "\n")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    cases = [
        (
            [[1, 1], [2, 3], [2, 3]],
            [[1, 1], [2, 3], [3, 2]],
        )
    ]
    rng = random.Random(31707)
    for n, m in [(1, 1), (1, 7), (2, 1), (2, 5), (3, 4),
                 (4, 7), (5, 2), (6, 9), (8, 8)]:
        answer = [[(row + column) % n + 1 for column in range(m)]
                  for row in range(n)]
        matrix = [row[:] for row in answer]
        for row in matrix:
            rng.shuffle(row)
        cases.append((matrix, answer))

    for index, (matrix, answer) in enumerate(cases):
        write_case(out_dir, index, matrix, answer)


if __name__ == "__main__":
    main()
