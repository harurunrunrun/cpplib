#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
import string
from pathlib import Path


MAX_LENGTH = 1000


def solve_brute(text: str) -> int:
    return sum(
        text[left:right] == text[left:right][::-1]
        for left in range(len(text))
        for right in range(left + 1, len(text) + 1)
    )


def write_case(
    out_dir: Path,
    name: str,
    text: str,
    answer: int | None = None,
    *,
    final_newline: bool = True,
) -> None:
    assert 1 <= len(text) <= MAX_LENGTH
    if answer is None:
        answer = solve_brute(text)
    (out_dir / f"{name}.in").write_text(
        text + ("\n" if final_newline else ""),
        encoding="ascii",
    )
    (out_dir / f"{name}.out").write_text(f"{answer}\n", encoding="ascii")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    write_case(out_dir, "official", "malayalam", 15)
    for index, text in enumerate(["a", "aa", "ab", "abacaba", "abcdefghijklmnopqrstuvwxyz"]):
        write_case(out_dir, f"fixed_{index:02d}", text)

    rng = random.Random(100000096)
    for case_index in range(12):
        text = "".join(rng.choice("abcde") for _ in range(rng.randint(1, 80)))
        write_case(out_dir, f"random_{case_index:02d}", text)

    write_case(
        out_dir,
        "maximum_all_equal",
        "a" * MAX_LENGTH,
        MAX_LENGTH * (MAX_LENGTH + 1) // 2,
    )
    write_case(
        out_dir,
        "maximum_periodic",
        (string.ascii_lowercase * 39)[:MAX_LENGTH],
        final_newline=False,
    )


if __name__ == "__main__":
    main()
