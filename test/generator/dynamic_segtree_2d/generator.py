#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path

LIMIT = 1024


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(20260713)
    values: dict[tuple[int, int], int] = {}
    commands: list[str] = []
    outputs: list[str] = []
    for _ in range(1700):
        kind = rng.randrange(100)
        if kind < 28:
            row = rng.randrange(LIMIT)
            col = rng.randrange(LIMIT)
            value = rng.randrange(-5000, 5001)
            commands.append(f"SET {row} {col} {value}")
            values[row, col] = value
        elif kind < 48:
            row = rng.randrange(LIMIT)
            col = rng.randrange(LIMIT)
            value = rng.randrange(-500, 501)
            commands.append(f"ADD {row} {col} {value}")
            values[row, col] = values.get((row, col), 0) + value
        elif kind < 60:
            row = rng.randrange(LIMIT)
            col = rng.randrange(LIMIT)
            commands.append(f"GET {row} {col}")
            outputs.append(str(values.get((row, col), 0)))
        elif kind < 88:
            row_begin = rng.randrange(LIMIT + 1)
            row_end = rng.randrange(LIMIT + 1)
            col_begin = rng.randrange(LIMIT + 1)
            col_end = rng.randrange(LIMIT + 1)
            if row_begin > row_end:
                row_begin, row_end = row_end, row_begin
            if col_begin > col_end:
                col_begin, col_end = col_end, col_begin
            commands.append(f"PROD {row_begin} {col_begin} {row_end} {col_end}")
            outputs.append(str(sum(
                value
                for (row, col), value in values.items()
                if row_begin <= row < row_end and col_begin <= col < col_end
            )))
        elif kind < 98:
            commands.append("ALL")
            outputs.append(str(sum(values.values())))
        else:
            commands.append("CLEAR")
            values.clear()

    (out_dir / "case_00.in").write_text(
        f"{len(commands)}\n" + "\n".join(commands) + "\n", encoding="utf-8"
    )
    (out_dir / "case_00.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
