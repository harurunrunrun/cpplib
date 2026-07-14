#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


MAX_LENGTH = 6_100
MANY_TEST_COUNT = 64


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


def validate(texts: list[str]) -> None:
    assert texts
    assert all(1 <= len(text) <= MAX_LENGTH for text in texts)
    assert all(text.isascii() and not any(char.isspace() for char in text) for text in texts)


def write_case(
    out_dir: Path,
    name: str,
    texts: list[str],
    answers: list[int] | None = None,
) -> None:
    validate(texts)
    if answers is None:
        answers = [solve_brute(text) for text in texts]
    assert len(answers) == len(texts)
    assert all(0 <= answer < len(text) for text, answer in zip(texts, answers))

    (out_dir / f"{name}.in").write_text(
        "\n".join([str(len(texts)), *texts]) + "\n",
        encoding="ascii",
    )
    (out_dir / f"{name}.out").write_text(
        "\n".join(map(str, answers)) + "\n",
        encoding="ascii",
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    write_case(out_dir, "official", ["fft"], [1])
    write_case(
        out_dir,
        "fixed",
        [
            "a",
            "abba",
            "abc",
            "Aa",
            "racecar",
            "google",
            "aabb",
            "abcdefghijklmnopqrstuvwxyz",
        ],
    )

    source = random.Random(1021097)
    for file_index in range(10):
        texts = [
            "".join(
                source.choice("abcABCxyzXYZ")
                for _ in range(source.randint(1, 36))
            )
            for _ in range(40)
        ]
        write_case(out_dir, f"random_{file_index:02d}", texts)

    many_texts = [
        "".join(
            source.choice("abcd")
            for _ in range(1 + test_index % 24)
        )
        for test_index in range(MANY_TEST_COUNT)
    ]
    write_case(out_dir, "many_test_cases", many_texts)

    write_case(
        out_dir,
        "max_length_palindrome",
        ["a" * MAX_LENGTH],
        [0],
    )
    write_case(
        out_dir,
        "max_length_two_blocks",
        ["a" * (MAX_LENGTH // 2) + "b" * (MAX_LENGTH // 2)],
        [MAX_LENGTH // 2],
    )


if __name__ == "__main__":
    main()
