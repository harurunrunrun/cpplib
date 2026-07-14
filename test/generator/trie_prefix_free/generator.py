#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def is_prefix_free_brute(words: list[str]) -> bool:
    return all(
        first_index == second_index or not second.startswith(first)
        for first_index, first in enumerate(words)
        for second_index, second in enumerate(words)
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    cases = [
        ["911", "97625999", "91125426"],
        ["113", "12340", "123440", "12345", "98346"],
        ["12", "123"],
        ["123", "12"],
        ["123", "123"],
        ["0"],
    ]
    source = random.Random(40330091)
    for _ in range(250):
        word_count = source.randint(1, 100)
        words = [
            "".join(source.choice("0123456789") for _ in range(source.randint(1, 10)))
            for _ in range(word_count)
        ]
        cases.append(words)

    input_lines = [str(len(cases))]
    output_lines = []
    for words in cases:
        input_lines.append(str(len(words)))
        input_lines.extend(words)
        output_lines.append("YES" if is_prefix_free_brute(words) else "NO")

    (out_dir / "case_00.in").write_text(
        "\n".join(input_lines) + "\n",
        encoding="ascii",
    )
    (out_dir / "case_00.out").write_text(
        "\n".join(output_lines) + "\n",
        encoding="ascii",
    )


if __name__ == "__main__":
    main()
