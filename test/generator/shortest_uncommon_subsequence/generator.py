#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def is_subsequence(candidate: str, text: str) -> bool:
    position = 0
    for character in text:
        if position < len(candidate) and candidate[position] == character:
            position += 1
    return position == len(candidate)


def shortest_uncommon_subsequence_brute(source: str, forbidden: str) -> int:
    best = len(source) + 1
    for mask in range(1, 1 << len(source)):
        candidate = "".join(
            source[index]
            for index in range(len(source))
            if mask >> index & 1
        )
        if len(candidate) < best and not is_subsequence(candidate, forbidden):
            best = len(candidate)
    return -1 if best > len(source) else best


def encode(text: str) -> str:
    return text if text else "-"


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    small_cases = [
        ("", ""),
        ("", "abc"),
        ("a", ""),
        ("a", "a"),
        ("a", "b"),
        ("aa", "a"),
        ("abc", "abc"),
        ("abc", "acb"),
        ("banana", "anbnaanbaan"),
        ("babab", "babba"),
    ]
    rng = random.Random(20260714)
    for _ in range(160):
        source = "".join(rng.choice("abcd") for _ in range(rng.randrange(13)))
        forbidden = "".join(rng.choice("abcd") for _ in range(rng.randrange(13)))
        small_cases.append((source, forbidden))

    cases_with_answers = [
        (source, forbidden, shortest_uncommon_subsequence_brute(source, forbidden))
        for source, forbidden in small_cases
    ]
    cases_with_answers.extend(
        [
            ("a" * 1000, "", 1),
            ("", "a" * 1000, -1),
            ("a" * 1000, "a" * 999, 1000),
            ("a" * 1000, "a" * 1000, -1),
            ("a" * 500 + "b" * 500, "b" * 500 + "a" * 500, 2),
            ("a" * 999 + "b", "a" * 1000, 1),
        ]
    )

    input_lines = [str(len(cases_with_answers))]
    output_lines: list[str] = []
    for source, forbidden, answer in cases_with_answers:
        input_lines.append(f"{encode(source)} {encode(forbidden)}")
        output_lines.append(str(answer))
    (out_dir / "case_00.in").write_text(
        "\n".join(input_lines) + "\n", encoding="ascii"
    )
    (out_dir / "case_00.out").write_text(
        "\n".join(output_lines) + "\n", encoding="ascii"
    )


if __name__ == "__main__":
    main()
