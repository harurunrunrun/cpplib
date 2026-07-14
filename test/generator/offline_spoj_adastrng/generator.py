#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
import string
from pathlib import Path


MAX_LENGTH = 300_000


def solve_brute(text: str) -> list[int]:
    distinct = {text[left:right] for left in range(len(text)) for right in range(left + 1, len(text) + 1)}
    return [sum(value[0] == letter for value in distinct) for letter in string.ascii_lowercase]


def write_case(
    out_dir: Path,
    name: str,
    text: str,
    answer: list[int] | None = None,
    *,
    final_newline: bool = True,
) -> None:
    assert 1 <= len(text) <= MAX_LENGTH
    assert all('a' <= character <= 'z' for character in text)
    if answer is None:
        answer = solve_brute(text)
    assert len(answer) == 26
    (out_dir / f"{name}.in").write_text(
        text + ("\n" if final_newline else ""),
        encoding="ascii",
    )
    (out_dir / f"{name}.out").write_text(
        " ".join(map(str, answer)) + "\n",
        encoding="ascii",
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    examples = [
        ("aaa", [3] + [0] * 25),
        ("abc", [3, 2, 1] + [0] * 23),
        ("aabbaa", [10, 6] + [0] * 24),
        ("acbabca", [10, 7, 7] + [0] * 23),
    ]
    for index, (text, answer) in enumerate(examples):
        write_case(out_dir, f"official_{index:02d}", text, answer)

    rng = random.Random(300000099)
    for case_index in range(15):
        text = "".join(rng.choice("abcdef") for _ in range(rng.randint(1, 75)))
        write_case(out_dir, f"random_{case_index:02d}", text)

    write_case(
        out_dir,
        "maximum_all_equal",
        "a" * MAX_LENGTH,
        [MAX_LENGTH] + [0] * 25,
    )

    periodic = (string.ascii_lowercase * (MAX_LENGTH // 26 + 1))[:MAX_LENGTH]
    write_case(
        out_dir,
        "maximum_alphabet_cycle",
        periodic,
        [MAX_LENGTH - offset for offset in range(26)],
        final_newline=False,
    )


if __name__ == "__main__":
    main()
