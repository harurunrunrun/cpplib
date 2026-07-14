#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import itertools
import random
from pathlib import Path


Case = tuple[str, int]


def brute_minimum_rotation(text: str) -> int:
    if not text:
        return 0
    return min(
        range(len(text)),
        key=lambda index: (text[index:] + text[:index], index),
    )


def write_cases(out_dir: Path, stem: str, cases: list[Case]) -> None:
    encoded = [text if text else "-" for text, _ in cases]
    (out_dir / f"{stem}.in").write_text(
        str(len(cases)) + "\n" + "\n".join(encoded) + "\n",
        encoding="ascii",
    )
    (out_dir / f"{stem}.out").write_text(
        "".join(f"{answer}\n" for _, answer in cases),
        encoding="ascii",
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    small_texts = [
        "",
        "a",
        "aaaaaa",
        "abcdef",
        "fedcba",
        "abababab",
        "babababa",
        "banana",
        "mississippi",
        "aaabaaa",
        "cabcab",
    ]
    for length in range(1, 10):
        small_texts.extend(
            "".join(characters)
            for characters in itertools.product("ab", repeat=length)
        )

    rng = random.Random(20260714093)
    for _ in range(500):
        length = rng.randrange(1, 121)
        alphabet = "abcde"[: rng.randrange(1, 6)]
        small_texts.append(
            "".join(rng.choice(alphabet) for _ in range(length))
        )

    small_cases = [
        (text, brute_minimum_rotation(text)) for text in small_texts
    ]
    for index in range(0, len(small_cases), 100):
        write_cases(
            out_dir,
            f"brute_{index // 100:02d}",
            small_cases[index:index + 100],
        )

    maximum_length = 500_000
    write_cases(
        out_dir,
        "large_linear",
        [
            ("a" * maximum_length, 0),
            ("ab" * (maximum_length // 2), 0),
            ("b" * (maximum_length - 1) + "a", maximum_length - 1),
        ],
    )


if __name__ == "__main__":
    main()
