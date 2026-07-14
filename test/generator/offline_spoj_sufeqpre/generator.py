#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


MAX_TEST_COUNT = 50
MAX_LENGTH = 1000000


def border_count_naive(text: str) -> int:
    return sum(
        text[:length] == text[len(text) - length :]
        for length in range(1, len(text))
    )


def write_suite(
    out_dir: Path,
    stem: str,
    texts: list[str],
    answers: list[int] | None = None,
    *,
    trailing_newline: bool = True,
) -> None:
    assert 1 <= len(texts) <= MAX_TEST_COUNT
    assert all(1 <= len(text) <= MAX_LENGTH for text in texts)
    if answers is None:
        answers = [border_count_naive(text) for text in texts]
    assert len(answers) == len(texts)
    input_data = "\n".join([str(len(texts)), *texts])
    if trailing_newline:
        input_data += "\n"
    (out_dir / f"{stem}.in").write_text(input_data, encoding="ascii")
    (out_dir / f"{stem}.out").write_text(
        "".join(
            f"Case {index}: {answer}\n"
            for index, answer in enumerate(answers, start=1)
        ),
        encoding="ascii",
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    write_suite(
        out_dir,
        "official_sample",
        ["ababab", "aaaa", "abcabc"],
    )
    write_suite(
        out_dir,
        "overlapping_borders",
        [
            "a",
            "aa",
            "aaa",
            "aaaaa",
            "ababa",
            "ababab",
            "abcabca",
            "aaaaabaaaaa",
            "abcde",
            "xyzxyzxyz",
        ],
    )

    rng = random.Random(92092)
    random_small = [
        "".join(rng.choice("abcde") for _ in range(rng.randint(1, 60)))
        for _ in range(MAX_TEST_COUNT)
    ]
    write_suite(out_dir, "maximum_test_count_bruteforce", random_small)

    # The last token ends exactly at EOF, with no trailing line break.
    write_suite(
        out_dir,
        "eof_after_last_case",
        ["abcabcabc"],
        trailing_newline=False,
    )

    maximum_texts = [
        "a" * MAX_LENGTH,
        "ab" * (MAX_LENGTH // 2),
        "a" * (MAX_LENGTH - 1) + "b",
    ]
    maximum_answers = [MAX_LENGTH - 1, MAX_LENGTH // 2 - 1, 0]
    write_suite(
        out_dir,
        "maximum_length",
        maximum_texts,
        maximum_answers,
    )


if __name__ == "__main__":
    main()
