#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def palindrome_lengths(text: str) -> list[int]:
    answer: list[int] = []
    for doubled_center in range(2 * len(text) - 1):
        if doubled_center % 2 == 0:
            left = right = doubled_center // 2
        else:
            left = doubled_center // 2
            right = left + 1
        while left >= 0 and right < len(text) and text[left] == text[right]:
            left -= 1
            right += 1
        answer.append(right - left - 1)
    return answer


def write_case(out_dir: Path, index: int, text: str) -> None:
    answer = palindrome_lengths(text)
    (out_dir / f"case_{index:02d}.in").write_text(text + "\n", encoding="utf-8")
    (out_dir / f"case_{index:02d}.out").write_text(
        " ".join(map(str, answer)) + "\n", encoding="utf-8"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    arguments = parser.parse_args()
    out_dir = Path(arguments.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    fixed = ["a", "aa", "abacaba", "aaaaaaa", "abcddcbae"]
    for index, text in enumerate(fixed):
        write_case(out_dir, index, text)

    rng = random.Random(20260714)
    for index in range(len(fixed), len(fixed) + 20):
        length = rng.randrange(1, 181)
        text = "".join(rng.choice("abcde") for _ in range(length))
        write_case(out_dir, index, text)


if __name__ == "__main__":
    main()
