#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def encode(value: str) -> str:
    return value if value else "-"


def suffix_array(value: str) -> list[int]:
    return sorted(range(len(value)), key=lambda index: value[index:])


def lcp_array(value: str, suffixes: list[int]) -> list[int]:
    result: list[int] = []
    for first, second in zip(suffixes, suffixes[1:]):
        length = 0
        while first + length < len(value) and second + length < len(value) and value[first + length] == value[second + length]:
            length += 1
        result.append(length)
    return result


def format_vector(values: list[int]) -> str:
    return " ".join(map(str, [len(values), *values]))


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(20260713)
    strings = ["", "a", "aaaaa", "banana", "mississippi", "abracadabra", "zyxwvutsrqponmlkjihgfedcba"]
    for _ in range(180):
        length = rng.randrange(101)
        strings.append("".join(rng.choice("abcde") for _ in range(length)))

    input_lines = [str(len(strings)), *(encode(value) for value in strings)]
    output_lines: list[str] = []
    for value in strings:
        suffixes = suffix_array(value)
        output_lines.append(format_vector(suffixes))
        output_lines.append(format_vector(lcp_array(value, suffixes)))

    (out_dir / "case_00.in").write_text("\n".join(input_lines) + "\n", encoding="utf-8")
    (out_dir / "case_00.out").write_text("\n".join(output_lines) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
