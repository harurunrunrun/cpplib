#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
import string
from pathlib import Path


def suffix_array_naive(text: str) -> list[int]:
    return sorted(range(len(text)), key=lambda index: text[index:])


def write_case(out_dir: Path, index: int, text: str) -> None:
    answer = suffix_array_naive(text)
    stem = f"case_{index:02d}"
    (out_dir / f"{stem}.in").write_text(text + "\n", encoding="ascii")
    (out_dir / f"{stem}.out").write_text(
        "".join(f"{position}\n" for position in answer),
        encoding="ascii",
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    cases = [
        "abracadabra0AbRa4Cad14abra",
        "0",
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
        "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz",
        "zZ9aA0zZ9aA0",
        "banana",
        "mississippi",
        "abababababababababababababababab",
    ]
    rng = random.Random(76076)
    alphabet = string.digits + string.ascii_uppercase + string.ascii_lowercase
    for length in (2, 3, 4, 7, 16, 31, 64, 127, 255):
        for _ in range(3):
            cases.append("".join(rng.choice(alphabet) for _ in range(length)))
    cases.append("".join(rng.choice("aAbB019zZ") for _ in range(2048)))

    for index, text in enumerate(cases):
        write_case(out_dir, index, text)


if __name__ == "__main__":
    main()
