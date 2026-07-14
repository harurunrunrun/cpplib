#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path


def rank_gf2(rows, column_count):
    values = [int(row, 2) if row else 0 for row in rows]
    rank = 0
    for bit in range(column_count - 1, -1, -1):
        pivot = next(
            (index for index in range(rank, len(values))
             if (values[index] >> bit) & 1),
            None,
        )
        if pivot is None:
            continue
        values[rank], values[pivot] = values[pivot], values[rank]
        for row in range(rank + 1, len(values)):
            if (values[row] >> bit) & 1:
                values[row] ^= values[rank]
        rank += 1
    return rank


def write_case(out_dir, index, row_count, column_count, rows):
    input_lines = [f"{row_count} {column_count}", *rows]
    (out_dir / f"case_{index:03d}.in").write_text(
        "\n".join(input_lines) + "\n", encoding="utf-8"
    )
    (out_dir / f"case_{index:03d}.out").write_text(
        f"{rank_gf2(rows, column_count)}\n", encoding="utf-8"
    )


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    cases = [
        (0, 0, []),
        (0, 9, []),
        (1, 1, ["0"]),
        (1, 1, ["1"]),
        (2, 2, ["10", "01"]),
        (3, 4, ["1010", "0110", "1100"]),
    ]
    rng = random.Random(0xF2A11)
    for row_count in range(1, 13):
        for column_count in range(1, 15):
            if rng.randrange(4) != 0:
                continue
            rows = [
                "".join(str(rng.randrange(2)) for _ in range(column_count))
                for _ in range(row_count)
            ]
            cases.append((row_count, column_count, rows))
    for index, case in enumerate(cases):
        write_case(out_dir, index, *case)


if __name__ == "__main__":
    main()
