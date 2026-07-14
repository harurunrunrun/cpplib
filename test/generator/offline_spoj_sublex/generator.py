#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def distinct_substrings_naive(text: str) -> list[str]:
    return sorted(
        {
            text[left:right]
            for left in range(len(text))
            for right in range(left + 1, len(text) + 1)
        }
    )


def write_case(
    out_dir: Path,
    name: str,
    text: str,
    queries: list[int],
    answers: list[str],
) -> None:
    assert len(queries) == len(answers)
    assert 1 <= len(text) <= 90_000
    assert 1 <= len(queries) <= 500
    assert all(0 < rank < 2**31 for rank in queries)
    (out_dir / f"{name}.in").write_text(
        "\n".join([text, str(len(queries)), *(str(rank) for rank in queries)]) + "\n",
        encoding="ascii",
    )
    (out_dir / f"{name}.out").write_text(
        "\n".join(answers) + "\n",
        encoding="ascii",
    )


def write_naive_case(
    out_dir: Path,
    name: str,
    text: str,
    rng: random.Random,
    query_count: int | None = None,
) -> None:
    ordered = distinct_substrings_naive(text)
    if query_count is None:
        ranks = [1, len(ordered), (len(ordered) + 1) // 2]
        if len(ordered) <= 32:
            ranks.extend(range(1, len(ordered) + 1))
        ranks.extend(rng.randint(1, len(ordered)) for _ in range(8))
    else:
        ranks = [
            (index * 37 + index * index * 11) % len(ordered) + 1
            for index in range(query_count)
        ]
        ranks[0] = 1
        ranks[-1] = len(ordered)
    write_case(out_dir, name, text, ranks, [ordered[rank - 1] for rank in ranks])


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(77077)
    fixed = [
        "a",
        "aaa",
        "banana",
        "mississippi",
        "abacaba",
        "abcdefghijklmnopqrstuvwxyz",
        "zyxwvutsrqponmlkjihgfedcba",
        "abababababababab",
        "abcabcabcabcabc",
        "qwertyuiopasdfghjklzxcvbnm",
    ]
    for index, text in enumerate(fixed):
        write_naive_case(out_dir, f"fixed_{index:02d}", text, rng)

    for length in (2, 3, 5, 8, 13, 21, 34):
        for alphabet in ("ab", "abc", "abcdef", "abcdefghijklmnopqrstuvwxyz"):
            text = "".join(rng.choice(alphabet) for _ in range(length))
            write_naive_case(
                out_dir,
                f"random_{length:02d}_{len(alphabet):02d}",
                text,
                rng,
            )

    # Exercise the maximum query count while retaining the independent naive
    # oracle used for all ordinary cases.
    write_naive_case(out_dir, "max_queries", "abracadabra", rng, query_count=500)

    # Maximum |S| boundary. For a unary string the distinct substrings are
    # exactly a, aa, ..., a^|S|, so the oracle is closed-form and independent.
    size = 90_000
    boundary_ranks = [1, 2, size // 2, size - 1, size]
    write_case(
        out_dir,
        "max_length_unary",
        "a" * size,
        boundary_ranks,
        ["a" * rank for rank in boundary_ranks],
    )


if __name__ == "__main__":
    main()
