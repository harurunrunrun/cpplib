#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def solve_brute(text: str) -> int:
    size = len(text)
    if size == 0:
        return 0
    dp = [[0] * size for _ in range(size)]
    for length in range(2, size + 1):
        for left in range(size - length + 1):
            right = left + length - 1
            if text[left] == text[right]:
                dp[left][right] = dp[left + 1][right - 1]
            else:
                dp[left][right] = 1 + min(
                    dp[left + 1][right], dp[left][right - 1]
                )
    return dp[0][size - 1]


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    texts = [
        "a",
        "abba",
        "fft",
        "abc",
        "Aa",
        "racecar",
        "google",
        "aabb",
        "abcdefghijklmnopqrstuvwxyz",
    ]
    source = random.Random(1021097)
    for _ in range(300):
        texts.append("".join(
            source.choice("abcABCxyzXYZ")
            for _ in range(source.randint(1, 32))
        ))

    (out_dir / "case_00.in").write_text(
        "\n".join([str(len(texts)), *texts]) + "\n",
        encoding="ascii",
    )
    (out_dir / "case_00.out").write_text(
        "\n".join(str(solve_brute(text)) for text in texts) + "\n",
        encoding="ascii",
    )


if __name__ == "__main__":
    main()
