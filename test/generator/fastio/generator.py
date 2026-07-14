#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
from pathlib import Path


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    records = [
        (-2147483648, -9223372036854775808, "minimum"),
        (2147483647, 9223372036854775807, "maximum"),
        (0, 0, "zero"),
        (-1, -1, "negative"),
        (1, 1, "positive"),
        (-123456789, 4000000000000000000, "mixed"),
        (987654321, -4000000000000000000, "reverse"),
    ]
    separators = ["\t", " ", "\r\n", "  ", "\n\t", " \r\n", "\t\t"]
    pieces = [str(len(records)), separators[0]]
    for index, (integer, long_integer, word) in enumerate(records):
        pieces.extend(
            [
                str(integer),
                separators[(index + 1) % len(separators)],
                str(long_integer),
                separators[(index + 2) % len(separators)],
                word,
                separators[(index + 3) % len(separators)],
            ]
        )
    expected = "".join(
        f"{word}:{integer}:{long_integer}\n"
        for integer, long_integer, word in records
    )
    (out_dir / "case_00.in").write_text("".join(pieces), encoding="utf-8")
    (out_dir / "case_00.out").write_text(expected, encoding="utf-8")

    # Cross both one-megabyte internal buffers so refill and flush paths are
    # checked in addition to scalar parsing and formatting.
    large_count = 70_000
    large_input = [f"{large_count}\n"]
    large_output: list[str] = []
    for index in range(large_count):
        integer = (index * 1_000_003) % 2_000_000_001 - 1_000_000_000
        long_integer = (
            (index * 1_000_000_007) % 8_000_000_000_000_000_001
            - 4_000_000_000_000_000_000
        )
        word = f"record{index % 97}"
        separator_a = separators[index % len(separators)]
        separator_b = separators[(index + 3) % len(separators)]
        separator_c = separators[(index + 5) % len(separators)]
        large_input.append(
            f"{integer}{separator_a}{long_integer}{separator_b}{word}{separator_c}"
        )
        large_output.append(f"{word}:{integer}:{long_integer}\n")
    (out_dir / "case_01.in").write_text("".join(large_input), encoding="utf-8")
    (out_dir / "case_01.out").write_text("".join(large_output), encoding="utf-8")


if __name__ == "__main__":
    main()
