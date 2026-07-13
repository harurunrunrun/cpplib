#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(20260713)
    n = 173
    values = [rng.randrange(-10**6, 10**6 + 1) for _ in range(n)]
    ranges = []
    outputs = []
    for _ in range(700):
        left = rng.randrange(n + 1)
        right = rng.randrange(n + 1)
        if left > right:
            left, right = right, left
        ranges.append((left, right))
        outputs.append(str(sum(values[left:right])))

    height, width = 19, 23
    matrix = [
        [rng.randrange(-10000, 10001) for _ in range(width)]
        for _ in range(height)
    ]
    rectangles = []
    for _ in range(700):
        row_begin = rng.randrange(height + 1)
        row_end = rng.randrange(height + 1)
        col_begin = rng.randrange(width + 1)
        col_end = rng.randrange(width + 1)
        if row_begin > row_end:
            row_begin, row_end = row_end, row_begin
        if col_begin > col_end:
            col_begin, col_end = col_end, col_begin
        rectangles.append((row_begin, col_begin, row_end, col_end))
        outputs.append(
            str(
                sum(
                    matrix[row][col]
                    for row in range(row_begin, row_end)
                    for col in range(col_begin, col_end)
                )
            )
        )

    lines = [f"{n} {len(ranges)}", " ".join(map(str, values))]
    lines.extend(f"{left} {right}" for left, right in ranges)
    lines.append(f"{height} {width} {len(rectangles)}")
    lines.extend(" ".join(map(str, row)) for row in matrix)
    lines.extend(" ".join(map(str, rectangle)) for rectangle in rectangles)
    (out_dir / "case_00.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (out_dir / "case_00.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
