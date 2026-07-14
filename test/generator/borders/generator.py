#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def border_lengths_naive(text: str) -> list[int]:
    return [
        length
        for length in range(1, len(text))
        if text[:length] == text[len(text) - length :]
    ]


def encode(text: str) -> str:
    return text if text else "-"


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    cases = [
        "",
        "a",
        "aa",
        "aaaa",
        "ababab",
        "ababa",
        "abcabc",
        "abcabca",
        "aaaaabaaaaa",
        "abcd",
    ]
    rng = random.Random(20260714)
    for _ in range(500):
        cases.append(
            "".join(rng.choice("abcd") for _ in range(rng.randrange(51)))
        )

    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for text in cases:
        lengths = border_lengths_naive(text)
        input_lines.append(encode(text))
        output_lines.append(" ".join(map(str, [len(lengths), *lengths])))
    (out_dir / "case_00.in").write_text(
        "\n".join(input_lines) + "\n", encoding="ascii"
    )
    (out_dir / "case_00.out").write_text(
        "\n".join(output_lines) + "\n", encoding="ascii"
    )


if __name__ == "__main__":
    main()
