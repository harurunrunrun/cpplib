#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def solve_brute(text: str) -> str:
    for added_count in range(len(text) + 1):
        candidate = text + text[:added_count][::-1]
        if candidate == candidate[::-1]:
            return candidate
    raise AssertionError("a full reversed copy must form a palindrome")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    texts = [
        "a",
        "ab",
        "abba",
        "aaaa",
        "Aa",
        "aA",
        "amanaplanacanal",
        "xyz",
        "abcc",
        "ABba",
    ]
    source = random.Random(114750090)
    for _ in range(250):
        texts.append("".join(
            source.choice("abABxyXY")
            for _ in range(source.randint(1, 36))
        ))

    (out_dir / "case_00.in").write_text(
        "\n".join([str(len(texts)), *texts]) + "\n",
        encoding="ascii",
    )
    (out_dir / "case_00.out").write_text(
        "\n".join(solve_brute(text) for text in texts) + "\n",
        encoding="ascii",
    )


if __name__ == "__main__":
    main()
