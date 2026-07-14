#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


MAX_TEST_COUNT = 20
MAX_LENGTH = 1000


def distinct_substrings_naive(text: str) -> int:
    """Count distinct non-empty substrings without using the library algorithm."""
    return len(
        {
            text[left:right]
            for left in range(len(text))
            for right in range(left + 1, len(text) + 1)
        }
    )


def write_case(
    out_dir: Path,
    stem: str,
    texts: list[str],
    answers: list[int],
) -> None:
    assert 1 <= len(texts) <= MAX_TEST_COUNT
    assert len(texts) == len(answers)
    assert all(1 <= len(text) <= MAX_LENGTH for text in texts)
    (out_dir / f"{stem}.in").write_text(
        f"{len(texts)}\n" + "".join(f"{text}\n" for text in texts),
        encoding="ascii",
    )
    (out_dir / f"{stem}.out").write_text(
        "".join(f"{answer}\n" for answer in answers),
        encoding="ascii",
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    fixed = [
        "C",
        "CCCCC",
        "ABABA",
        "ABCDEFG",
        "AAAABAAA",
        "BANANA",
        "MISSISSIPPI",
        "ABCABCABC",
        "ZYXWVUTSRQPONMLKJIHGFEDCBA",
    ]
    write_case(
        out_dir,
        "fixed",
        fixed,
        [distinct_substrings_naive(text) for text in fixed],
    )

    rng = random.Random(20260714)
    random_small: list[str] = []
    alphabets = ("AB", "ABC", "ABCDE", "ABCDEFGHIJKLMNOPQRSTUVWXYZ")
    for index in range(MAX_TEST_COUNT):
        length = rng.randint(1, 18)
        alphabet = alphabets[index % len(alphabets)]
        random_small.append("".join(rng.choice(alphabet) for _ in range(length)))
    write_case(
        out_dir,
        "random_small",
        random_small,
        [distinct_substrings_naive(text) for text in random_small],
    )

    # Exercise both official maxima (20 strings of length 1000).  The expected
    # values use closed forms independent of suffix-array/LCP implementations:
    # a unary string has N substrings, while an alternating binary string has
    # two substrings of every length below N and one of length N.
    unary = [chr(ord("A") + index) * MAX_LENGTH for index in range(10)]
    pairs = [
        chr(ord("A") + 2 * index) + chr(ord("B") + 2 * index)
        for index in range(10)
    ]
    alternating = [pair * (MAX_LENGTH // 2) for pair in pairs]
    maximum = unary + alternating
    maximum_answers = [MAX_LENGTH] * len(unary) + [2 * MAX_LENGTH - 1] * len(
        alternating
    )
    write_case(out_dir, "maximum_constraints", maximum, maximum_answers)


if __name__ == "__main__":
    main()
