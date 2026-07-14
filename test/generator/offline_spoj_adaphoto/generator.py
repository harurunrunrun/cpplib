#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


ALPHABET = "^v-~"
MAX_LENGTH = 1_000_000


def longest_common_substring_brute(first: str, second: str) -> int:
    result = 0
    for left in range(len(first)):
        for right in range(len(second)):
            length = 0
            while (
                left + length < len(first)
                and right + length < len(second)
                and first[left + length] == second[right + length]
            ):
                length += 1
            result = max(result, length)
    return result


def write_case(
    out_dir: Path,
    name: str,
    first: str,
    second: str,
    answer: int | None = None,
    *,
    final_newline: bool = True,
) -> None:
    assert 1 <= len(first) <= MAX_LENGTH
    assert 1 <= len(second) <= MAX_LENGTH
    assert set(first) <= set(ALPHABET)
    assert set(second) <= set(ALPHABET)
    if answer is None:
        answer = longest_common_substring_brute(first, second)
    (out_dir / f"{name}.in").write_text(
        f"{first}\n{second}" + ("\n" if final_newline else ""),
        encoding="ascii",
    )
    (out_dir / f"{name}.out").write_text(f"{answer}\n", encoding="ascii")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    examples = [
        ("-~^v-", "^--v", 1),
        ("-~", "-~v^^^^^v-", 2),
        ("~-~-v--~^", "~^--^~-v", 3),
        ("^^^v-v^^v-v-v~v-", "vv^v-^~~^v^~^vv~^^-", 3),
        ("~~~vv~-~~vv~v-v--~-^-~^~-^^", "~^~--v~-", 4),
    ]
    for index, (first, second, answer) in enumerate(examples):
        write_case(out_dir, f"official_{index:02d}", first, second, answer)

    edge_cases = [
        ("^", "^"),
        ("^", "v"),
        ("^^^^", "^^"),
        ("^v-~", "~^v"),
    ]
    for index, (first, second) in enumerate(edge_cases):
        write_case(out_dir, f"edge_{index:02d}", first, second)

    rng = random.Random(20260714)
    for index in range(12):
        first = "".join(rng.choice(ALPHABET) for _ in range(rng.randint(1, 45)))
        second = "".join(rng.choice(ALPHABET) for _ in range(rng.randint(1, 45)))
        write_case(out_dir, f"random_{index:02d}", first, second)

    write_case(
        out_dir,
        "without_final_newline",
        "~^v-~^",
        "v-~",
        final_newline=False,
    )

    shared = "^-" * 200_000
    write_case(
        out_dir,
        "total_length_one_million",
        shared,
        "v" * 200_000 + shared,
        400_000,
    )

    maximum_rng = random.Random(7777777)
    maximum = "".join(maximum_rng.choice(ALPHABET) for _ in range(MAX_LENGTH))
    write_case(out_dir, "both_lengths_one_million", maximum, maximum, MAX_LENGTH)


if __name__ == "__main__":
    main()
