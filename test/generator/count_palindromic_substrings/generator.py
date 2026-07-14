#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def solve_brute(text: str) -> int:
    return sum(
        text[left:right] == text[left:right][::-1]
        for left in range(len(text))
        for right in range(left + 1, len(text) + 1)
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    texts = ["a", "aaa", "abac", "Aa", "malayalam", "abcdef"]
    rng = random.Random(100000096)
    for _ in range(350):
        texts.append("".join(rng.choice("abcABC") for _ in range(rng.randint(1, 45))))

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
