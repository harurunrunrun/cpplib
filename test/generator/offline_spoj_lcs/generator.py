#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def solve_dp(first: str, second: str) -> int:
    """Independent O(|first||second|) longest-common-substring oracle."""
    if len(second) > len(first):
        first, second = second, first
    previous = [0] * (len(second) + 1)
    answer = 0
    for first_character in first:
        current = [0] * (len(second) + 1)
        for index, second_character in enumerate(second, 1):
            if first_character == second_character:
                current[index] = previous[index - 1] + 1
                answer = max(answer, current[index])
        previous = current
    return answer


def write_case(
    out_dir: Path,
    name: str,
    first: str,
    second: str,
    answer: int | None = None,
) -> None:
    assert 1 <= len(first) <= 250_000
    assert 1 <= len(second) <= 250_000
    assert all("a" <= character <= "z" for character in first + second)
    if answer is None:
        answer = solve_dp(first, second)
    assert 0 <= answer <= min(len(first), len(second))
    (out_dir / f"{name}.in").write_text(
        first + "\n" + second + "\n",
        encoding="ascii",
    )
    (out_dir / f"{name}.out").write_text(
        f"{answer}\n",
        encoding="ascii",
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    fixed = (
        ("sample", "abcdxyz", "xyzabcd"),
        ("single_equal", "q", "q"),
        ("single_different", "a", "z"),
        ("identical", "bananabandana", "bananabandana"),
        ("contained", "xxcompetitiveprogrammingyy", "competitiveprogramming"),
        ("disjoint", "aaaaaaaa", "bbbbbbbb"),
        ("periodic", "abcabcabcabca", "bcabcabxabcabc"),
        ("small_many_clones", "abbbbbbbbbbbbb", "bbbbbbbbbbbbbb"),
        ("overlap_restart", "zxabcdezy", "yzabcdezx"),
        ("all_letters", "abcdefghijklmnopqrstuvwxyz", "zyxwvutsrqponmlkjihgfedcba"),
    )
    for name, first, second in fixed:
        write_case(out_dir, name, first, second)

    source = random.Random(790079)
    for index in range(12):
        alphabet = "ab" if index < 4 else "abcd" if index < 8 else "abcdefgh"
        first = "".join(
            source.choice(alphabet) for _ in range(source.randint(1, 80))
        )
        second = "".join(
            source.choice(alphabet) for _ in range(source.randint(1, 80))
        )
        write_case(out_dir, f"random_{index:02d}", first, second)

    size = 250_000
    maximum = ("abcdefghijklmnopqrstuvwxyz" * ((size + 25) // 26))[:size]
    write_case(
        out_dir,
        "max_length_identical",
        maximum,
        maximum,
        size,
    )
    write_case(
        out_dir,
        "max_length_many_clones",
        "a" + "b" * (size - 1),
        "b" * size,
        size - 1,
    )


if __name__ == "__main__":
    main()
