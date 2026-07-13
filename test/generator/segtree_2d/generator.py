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
    height, width = 23, 27
    initial = [
        [rng.randrange(-1000, 1001) for _ in range(width)]
        for _ in range(height)
    ]
    values = [row[:] for row in initial]
    commands: list[str] = []
    outputs: list[str] = []
    for _ in range(1400):
        kind = rng.randrange(5)
        if kind == 0:
            row = rng.randrange(height)
            col = rng.randrange(width)
            value = rng.randrange(-5000, 5001)
            commands.append(f"SET {row} {col} {value}")
            values[row][col] = value
        elif kind == 1:
            row = rng.randrange(height)
            col = rng.randrange(width)
            value = rng.randrange(-500, 501)
            commands.append(f"ADD {row} {col} {value}")
            values[row][col] += value
        elif kind == 2:
            row = rng.randrange(height)
            col = rng.randrange(width)
            commands.append(f"GET {row} {col}")
            outputs.append(str(values[row][col]))
        elif kind == 3:
            row_begin = rng.randrange(height + 1)
            row_end = rng.randrange(height + 1)
            col_begin = rng.randrange(width + 1)
            col_end = rng.randrange(width + 1)
            if row_begin > row_end:
                row_begin, row_end = row_end, row_begin
            if col_begin > col_end:
                col_begin, col_end = col_end, col_begin
            commands.append(f"PROD {row_begin} {col_begin} {row_end} {col_end}")
            outputs.append(str(sum(
                values[row][col]
                for row in range(row_begin, row_end)
                for col in range(col_begin, col_end)
            )))
        else:
            commands.append("ALL")
            outputs.append(str(sum(map(sum, values))))

    lines = [f"{height} {width} {len(commands)}"]
    lines.extend(" ".join(map(str, row)) for row in initial)
    lines.extend(commands)
    (out_dir / "case_00.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (out_dir / "case_00.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
