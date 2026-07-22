#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
import string
from pathlib import Path


def solve_brute(text: str) -> int:
    return len(
        {
            text[left:right]
            for left in range(len(text))
            for right in range(left + 1, len(text) + 1)
        }
    )


def write_case(
    out_dir: Path,
    index: int,
    texts: list[str],
    answers: list[int],
) -> None:
    stem = f"case_{index:02d}"
    (out_dir / f"{stem}.in").write_text(
        "\n".join([str(len(texts)), *texts]) + "\n",
        encoding="ascii",
    )
    (out_dir / f"{stem}.out").write_text(
        "\n".join(map(str, answers)) + "\n",
        encoding="ascii",
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    texts = ["a", "aaa", "banana", "mississippi", "abcabcabc"]
    rng = random.Random(20260722)
    for _ in range(300):
        texts.append(
            "".join(
                rng.choice("abcdef")
                for _ in range(rng.randint(1, 45))
            )
        )
    write_case(out_dir, 0, texts, [solve_brute(text) for text in texts])

    length = 300_000
    period = len(string.ascii_lowercase)
    periodic = (
        string.ascii_lowercase * (length // period + 1)
    )[:length]
    write_case(
        out_dir,
        1,
        ["a" * length, periodic],
        [length, period * length - period * (period - 1) // 2],
    )


if __name__ == "__main__":
    main()
