#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
import string
from pathlib import Path


def solve_brute(text: str) -> list[int]:
    distinct = {text[left:right] for left in range(len(text)) for right in range(left + 1, len(text) + 1)}
    return [sum(value[0] == letter for value in distinct) for letter in string.ascii_lowercase]


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    texts = ["a", "aaa", "abc", "aabbaa", "acbabca"]
    rng = random.Random(300000099)
    for _ in range(300):
        texts.append("".join(rng.choice("abcde") for _ in range(rng.randint(1, 45))))

    (out_dir / "case_00.in").write_text(
        "\n".join([str(len(texts)), *texts]) + "\n",
        encoding="ascii",
    )
    (out_dir / "case_00.out").write_text(
        "\n".join(" ".join(map(str, solve_brute(text))) for text in texts) + "\n",
        encoding="ascii",
    )


if __name__ == "__main__":
    main()
