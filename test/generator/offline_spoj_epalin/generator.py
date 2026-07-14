#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


MAX_LENGTH = 100_000


def solve_brute(text: str) -> str:
    for added_count in range(len(text) + 1):
        candidate = text + text[:added_count][::-1]
        if candidate == candidate[::-1]:
            return candidate
    raise AssertionError("a full reversed copy must form a palindrome")


def validate(texts: list[str]) -> None:
    assert texts
    assert all(1 <= len(text) <= MAX_LENGTH for text in texts)
    assert all(text.isascii() and text.isalpha() for text in texts)


def write_case(
    out_dir: Path,
    name: str,
    texts: list[str],
    answers: list[str] | None = None,
) -> None:
    validate(texts)
    if answers is None:
        answers = [solve_brute(text) for text in texts]
    assert len(answers) == len(texts)
    assert all(answer.startswith(text) for text, answer in zip(texts, answers))
    assert all(answer == answer[::-1] for answer in answers)

    (out_dir / f"{name}.in").write_text(
        "\n".join(texts) + "\n",
        encoding="ascii",
    )
    (out_dir / f"{name}.out").write_text(
        "\n".join(answers) + "\n",
        encoding="ascii",
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    write_case(
        out_dir,
        "official",
        ["aaaa", "abba", "amanaplanacanal", "xyz"],
        ["aaaa", "abba", "amanaplanacanalpanama", "xyzyx"],
    )
    write_case(
        out_dir,
        "fixed_and_case_sensitive",
        ["a", "ab", "Aa", "aA", "ABba", "abcc", "racecar", "XyZ"],
    )

    source = random.Random(114750090)
    for case_index in range(12):
        texts = [
            "".join(
                source.choice("abcABCxyzXYZ")
                for _ in range(source.randint(1, 45))
            )
            for _ in range(30)
        ]
        write_case(out_dir, f"random_{case_index:02d}", texts)

    crlf_texts = ["Aa", "abba", "xyz", "aBA"]
    crlf_answers = [solve_brute(text) for text in crlf_texts]
    (out_dir / "crlf_without_final_newline.in").write_bytes(
        "\r\n".join(crlf_texts).encode("ascii")
    )
    (out_dir / "crlf_without_final_newline.out").write_text(
        "\n".join(crlf_answers) + "\n",
        encoding="ascii",
    )

    already_palindrome = "a" * MAX_LENGTH
    longest_extension = "a" * (MAX_LENGTH - 1) + "b"
    write_case(
        out_dir,
        "max_length",
        [already_palindrome, longest_extension],
        [already_palindrome, longest_extension + "a" * (MAX_LENGTH - 1)],
    )


if __name__ == "__main__":
    main()
