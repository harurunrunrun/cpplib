#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


MAX_STRINGS = 10
MAX_LENGTH = 100_000


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


def write_case(
    out_dir: Path,
    name: str,
    texts: list[str],
    answer: int | None = None,
) -> None:
    assert 1 <= len(texts) <= MAX_STRINGS
    assert all(1 <= len(text) <= MAX_LENGTH for text in texts)
    if answer is None:
        answer = solve_naive(texts)
    (out_dir / f"{name}.in").write_text(
        "\n".join(texts) + "\n", encoding="ascii"
    )
    (out_dir / f"{name}.out").write_text(f"{answer}\n", encoding="ascii")


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
    ]
    for index, texts in enumerate(fixed):
        write_case(out_dir, f"fixed_{index:02d}", texts)

    for seed in range(12):
        rng = random.Random(800200 + seed)
        count = rng.randint(2, MAX_STRINGS)
        texts = [
            "".join(rng.choice("abcde") for _ in range(rng.randint(1, 18)))
            for _ in range(count)
        ]
        write_case(out_dir, f"random_{seed:02d}", texts)

    size = MAX_LENGTH
    write_case(
        out_dir,
        "maximum_strings_and_clones",
        ["a" + "b" * (size - 1), *(["b" * size] * 9)],
        size - 1,
    )
    repeated = ("abcdefghijklmnopqrstuvwxyz" * ((size + 25) // 26))[:size]
    write_case(out_dir, "maximum_identical", [repeated] * MAX_STRINGS, size)
    write_case(out_dir, "maximum_single", ["a" * size], size)
    write_case(out_dir, "maximum_disjoint", ["a" * size, "b" * size], 0)


if __name__ == "__main__":
    main()
