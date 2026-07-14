#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def solve_brute(text: str, pattern: str) -> int:
    return min(
        sum(left != right for left, right in zip(text[index:], pattern))
        for index in range(len(text) - len(pattern) + 1)
    )


def write_case(out_dir: Path, case_index: int, text: str, pattern: str) -> None:
    name = f"case_{case_index:03d}"
    (out_dir / f"{name}.in").write_text(
        f"{text}\n{pattern}\n", encoding="ascii"
    )
    (out_dir / f"{name}.out").write_text(
        f"{solve_brute(text, pattern)}\n", encoding="ascii"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    cases = [
        ("0", "0"),
        ("0", "1"),
        ("0001", "101"),
        ("0101010", "1010101"),
        ("10101000010011011110", "0010011111"),
        ("0" * 64, "1" * 31),
        ("01" * 64, "10" * 32),
    ]
    source = random.Random(196196)
    for _ in range(120):
        text_length = source.randint(1, 80)
        pattern_length = source.randint(1, text_length)
        text = "".join(source.choice("01") for _ in range(text_length))
        pattern = "".join(source.choice("01") for _ in range(pattern_length))
        cases.append((text, pattern))

    for case_index, case in enumerate(cases):
        write_case(out_dir, case_index, *case)


if __name__ == "__main__":
    main()
