#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
import string
from pathlib import Path


def brute_minimum_rotation(text: str) -> int:
    assert text
    return min(
        range(len(text)),
        key=lambda index: (text[index:] + text[:index], index),
    )


def write_case(
    out_dir: Path,
    stem: str,
    text: str,
    answer: int,
    *,
    final_newline: bool = True,
) -> None:
    assert 1 <= len(text) <= 100_000
    assert set(text) <= set(string.ascii_lowercase)
    input_data = text + ("\n" if final_newline else "")
    (out_dir / f"{stem}.in").write_text(input_data, encoding="ascii")
    (out_dir / f"{stem}.out").write_text(f"{answer}\n", encoding="ascii")


def write_bruteforce_case(
    out_dir: Path,
    stem: str,
    text: str,
    *,
    final_newline: bool = True,
) -> None:
    write_case(
        out_dir,
        stem,
        text,
        brute_minimum_rotation(text),
        final_newline=final_newline,
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    write_case(out_dir, "official_sample", "mississippi", 10)

    periodic = [
        "a",
        "aa",
        "a" * 31,
        "abab",
        "baba",
        "abababababab",
        "babababababa",
        "abcabcabc",
        "bcabcabca",
        "cabcaabcabca",
    ]
    for index, text in enumerate(periodic):
        write_bruteforce_case(out_dir, f"periodic_{index:02d}", text)

    rng = random.Random(202607140094)
    for index in range(60):
        length = rng.randrange(1, 201)
        alphabet = string.ascii_lowercase[: rng.randrange(1, 10)]
        text = "".join(rng.choice(alphabet) for _ in range(length))
        write_bruteforce_case(out_dir, f"random_{index:02d}", text)

    write_bruteforce_case(
        out_dir,
        "no_final_newline",
        "thelastlinehasnonewline",
        final_newline=False,
    )

    maximum_length = 100_000
    write_case(out_dir, "maximum_all_equal", "a" * maximum_length, 0)
    write_case(
        out_dir,
        "maximum_periodic_tie",
        "ba" * (maximum_length // 2),
        1,
    )
    write_case(
        out_dir,
        "maximum_last_is_minimum",
        "z" * (maximum_length - 1) + "a",
        maximum_length - 1,
    )
    write_case(
        out_dir,
        "maximum_long_equal_prefix",
        "b" + "a" * (maximum_length - 1),
        1,
    )


if __name__ == "__main__":
    main()
