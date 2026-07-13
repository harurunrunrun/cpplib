#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import itertools
import random
from pathlib import Path


def factorization(text: str) -> list[int]:
    boundaries = [0]
    begin = 0
    while begin < len(text):
        repeated = begin
        cursor = begin + 1
        while cursor < len(text) and text[repeated] <= text[cursor]:
            if text[repeated] < text[cursor]:
                repeated = begin
            else:
                repeated += 1
            cursor += 1
        length = cursor - repeated
        while begin <= repeated:
            begin += length
            boundaries.append(begin)
    return boundaries


def write_case(out_dir: Path, index: int, texts: list[str]) -> None:
    encoded = [text if text else "-" for text in texts]
    input_data = str(len(texts)) + "\n" + "\n".join(encoded) + "\n"
    output_data = "".join(
        " ".join(map(str, factorization(text))) + "\n" for text in texts
    )
    (out_dir / f"case_{index:02d}.in").write_text(input_data, encoding="utf-8")
    (out_dir / f"case_{index:02d}.out").write_text(output_data, encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)

    texts = [
        "", "a", "aaaaaa", "abcdef", "fedcba", "ababbab", "banana",
        "mississippi", "abababababab", "zzzaazzza", "aababaabb",
    ]
    for length in range(1, 9):
        texts.extend("".join(bits) for bits in itertools.product("ab", repeat=length))

    random_source = random.Random(2026071301)
    for _ in range(300):
        length = random_source.randrange(1, 300)
        alphabet = "abcde"[: random_source.randrange(1, 6)]
        texts.append("".join(random_source.choice(alphabet) for _ in range(length)))
    texts.append("a" * 499999 + "b")

    for index in range(0, len(texts), 75):
        write_case(args.out_dir, index // 75, texts[index:index + 75])


if __name__ == "__main__":
    main()
