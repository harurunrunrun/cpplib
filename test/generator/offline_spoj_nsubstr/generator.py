#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from collections import Counter
from pathlib import Path


def solve_naive(text: str) -> list[int]:
    return [
        max(Counter(text[index:index + length] for index in range(len(text) - length + 1)).values())
        for length in range(1, len(text) + 1)
    ]


def write_case(out_dir: Path, name: str, text: str, answer: list[int] | None = None) -> None:
    assert 1 <= len(text) <= 250_000
    assert all("a" <= c <= "z" for c in text)
    if answer is None:
        answer = solve_naive(text)
    assert len(answer) == len(text)
    (out_dir / f"{name}.in").write_text(text + "\n", encoding="ascii")
    (out_dir / f"{name}.out").write_text(
        "\n".join(map(str, answer)) + "\n",
        encoding="ascii",
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    for name, text in (
        ("single", "a"),
        ("sample", "ababa"),
        ("unary", "a" * 64),
        ("periodic", "abcababcababcab"),
        ("all_letters", "abcdefghijklmnopqrstuvwxyz"),
    ):
        write_case(out_dir, name, text)

    rng = random.Random(780078)
    for index, (length, alphabet) in enumerate(((37, "ab"), (71, "abc"), (96, "abcdef"))):
        text = "".join(rng.choice(alphabet) for _ in range(length))
        write_case(out_dir, f"random_{index:02d}", text)

    size = 250_000
    write_case(
        out_dir,
        "max_length_unary",
        "a" * size,
        [size - length + 1 for length in range(1, size + 1)],
    )
    write_case(
        out_dir,
        "max_length_many_clones",
        "a" + "b" * (size - 1),
        [max(1, size - length) for length in range(1, size + 1)],
    )


if __name__ == "__main__":
    main()
