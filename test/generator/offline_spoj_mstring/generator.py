#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


MAX_LENGTH = 1000


def is_subsequence(candidate: str, text: str) -> bool:
    position = 0
    for character in text:
        if position < len(candidate) and candidate[position] == character:
            position += 1
    return position == len(candidate)


def shortest_uncommon_subsequence_brute(source: str, forbidden: str) -> int:
    best = len(source) + 1
    for mask in range(1, 1 << len(source)):
        candidate = "".join(
            source[index]
            for index in range(len(source))
            if mask >> index & 1
        )
        if len(candidate) < best and not is_subsequence(candidate, forbidden):
            best = len(candidate)
    return -1 if best > len(source) else best


def write_case(
    out_dir: Path,
    stem: str,
    source: str,
    forbidden: str,
    answer: int,
) -> None:
    assert 1 <= len(source) <= MAX_LENGTH
    assert 1 <= len(forbidden) <= MAX_LENGTH
    assert 1 <= answer <= len(source)
    (out_dir / f"{stem}.in").write_text(
        f"{source}\n{forbidden}\n", encoding="ascii"
    )
    (out_dir / f"{stem}.out").write_text(f"{answer}\n", encoding="ascii")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    fixed = [
        ("banana", "anbnaanbaan"),
        ("babab", "babba"),
        ("a", "b"),
        ("aa", "a"),
        ("abc", "ab"),
        ("abc", "acb"),
        ("mississippi", "imissisippi"),
    ]
    for index, (source, forbidden) in enumerate(fixed):
        answer = shortest_uncommon_subsequence_brute(source, forbidden)
        assert answer != -1
        write_case(out_dir, f"fixed_{index:02d}", source, forbidden, answer)

    rng = random.Random(83083)
    random_cases: list[tuple[str, str, int]] = []
    seen: set[tuple[str, str]] = set(fixed)
    while len(random_cases) < 80:
        source = "".join(rng.choice("abcd") for _ in range(rng.randint(1, 12)))
        forbidden = "".join(rng.choice("abcd") for _ in range(rng.randint(1, 12)))
        if (source, forbidden) in seen:
            continue
        seen.add((source, forbidden))
        answer = shortest_uncommon_subsequence_brute(source, forbidden)
        if answer != -1:
            random_cases.append((source, forbidden, answer))
    for index, (source, forbidden, answer) in enumerate(random_cases):
        write_case(out_dir, f"random_{index:02d}", source, forbidden, answer)

    # Closed-form boundary cases, independent of the C++ DP implementation.
    # They exercise length 1000, an answer of 1000, and ordering/count effects.
    boundary_cases = [
        ("a" * 1000, "a" * 999, 1000),
        ("a" * 1000, "a" * 999 + "b", 1000),
        ("a" * 500 + "b" * 500, "b" * 500 + "a" * 500, 2),
        ("a" * 999 + "b", "a" * 1000, 1),
        ("z", "a" * 1000, 1),
        ("a" * 500 + "b" * 500, "a" * 500 + "b" * 499 + "a", 500),
    ]
    for index, (source, forbidden, answer) in enumerate(boundary_cases):
        write_case(out_dir, f"boundary_{index:02d}", source, forbidden, answer)


if __name__ == "__main__":
    main()
