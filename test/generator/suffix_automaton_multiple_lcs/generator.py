#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def solve_naive(texts: list[str]) -> int:
    shortest = min(texts, key=len)
    return max(
        (
            right - left
            for left in range(len(shortest))
            for right in range(left + 1, len(shortest) + 1)
            if all(shortest[left:right] in text for text in texts)
        ),
        default=0,
    )


def write_case(out_dir: Path, name: str, cases: list[list[str]]) -> None:
    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for texts in cases:
        assert 1 <= len(texts) <= 8
        assert all(1 <= len(text) <= 100 for text in texts)
        input_lines.append(str(len(texts)))
        input_lines.extend(texts)
        output_lines.append(str(solve_naive(texts)))
    (out_dir / f"{name}.in").write_text(
        "\n".join(input_lines) + "\n", encoding="ascii"
    )
    (out_dir / f"{name}.out").write_text(
        "\n".join(output_lines) + "\n", encoding="ascii"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    fixed = [
        ["a"],
        ["abc", "abc"],
        ["ababa", "babab", "cababa"],
        ["abc", "def", "ghi"],
        ["mississippi", "dismissal", "mission"],
        ["abcabcabc", "bcabcab", "zabcabz", "xxbcabcxx"],
        ["a" + "b" * 99, "b" * 100, "x" + "b" * 98 + "y"],
    ]
    write_case(out_dir, "fixed", fixed)

    for seed in range(5):
        rng = random.Random(80020 + seed)
        cases: list[list[str]] = []
        for _ in range(25):
            count = rng.randint(1, 6)
            cases.append([
                "".join(rng.choice("abcd") for _ in range(rng.randint(1, 24)))
                for _ in range(count)
            ])
        write_case(out_dir, f"random_{seed:02d}", cases)


if __name__ == "__main__":
    main()
