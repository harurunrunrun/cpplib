#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


MAX_LENGTH = 30_000


def solve_brute(text: str, length: int) -> int:
    return sum(
        text[start:start + length] == text[start:start + length][::-1]
        for start in range(len(text) - length + 1)
    )


def write_case(
    out_dir: Path,
    name: str,
    length: int,
    text: str,
    answer: int | None = None,
    *,
    final_newline: bool = True,
) -> None:
    assert 2 <= length <= len(text) <= MAX_LENGTH
    if answer is None:
        answer = solve_brute(text, length)
    suffix = "\n" if final_newline else ""
    (out_dir / f"{name}.in").write_text(
        f"{length}\n{text}{suffix}",
        encoding="ascii",
    )
    (out_dir / f"{name}.out").write_text(f"{answer}\n", encoding="ascii")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    write_case(out_dir, "official", 5, "ababab", 2)
    fixed = [
        (2, "aa"),
        (2, "abcdef"),
        (3, "ababa"),
        (6, "abccba"),
        (7, "racecarx"),
    ]
    for index, (length, text) in enumerate(fixed):
        write_case(out_dir, f"fixed_{index:02d}", length, text)

    rng = random.Random(30000095)
    for case_index in range(12):
        size = rng.randint(2, 90)
        text = "".join(rng.choice("abcd") for _ in range(size))
        length = rng.randint(2, size)
        write_case(out_dir, f"random_{case_index:02d}", length, text)

    write_case(out_dir, "maximum_all_equal", 2, "a" * MAX_LENGTH, MAX_LENGTH - 1)
    write_case(out_dir, "maximum_whole", MAX_LENGTH, "a" * MAX_LENGTH, 1)
    write_case(
        out_dir,
        "maximum_alternating",
        MAX_LENGTH - 1,
        "ab" * (MAX_LENGTH // 2),
        2,
        final_newline=False,
    )


if __name__ == "__main__":
    main()
