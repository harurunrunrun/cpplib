#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def determinant(rows: list[str]) -> int:
    size = len(rows)
    values = [int(row, 2) if row else 0 for row in rows]
    for column in range(size):
        bit = 1 << (size - 1 - column)
        pivot = next((row for row in range(column, size) if values[row] & bit), None)
        if pivot is None:
            return 0
        values[column], values[pivot] = values[pivot], values[column]
        for row in range(column + 1, size):
            if values[row] & bit:
                values[row] ^= values[column]
    return 1


def write_case(out_dir: Path, index: int, rows: list[str]) -> None:
    name = f"case_{index:03d}"
    (out_dir / f"{name}.in").write_text(
        str(len(rows)) + "\n" + "\n".join(rows) + ("\n" if rows else ""),
        encoding="ascii",
    )
    (out_dir / f"{name}.out").write_text(
        f"{determinant(rows)}\n", encoding="ascii"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    write_case(out_dir, 0, [])
    write_case(out_dir, 1, ["1"])
    write_case(out_dir, 2, ["11", "11"])
    write_case(out_dir, 3, ["100", "010", "001"])
    rng = random.Random(2026071401)
    for index in range(4, 34):
        size = rng.randrange(1, 91)
        rows = [
            "".join(str(rng.randrange(2)) for _ in range(size))
            for _ in range(size)
        ]
        write_case(out_dir, index, rows)
    size = 129
    write_case(
        out_dir,
        34,
        ["0" * row + "1" + "0" * (size - row - 1) for row in range(size)],
    )


if __name__ == "__main__":
    main()
