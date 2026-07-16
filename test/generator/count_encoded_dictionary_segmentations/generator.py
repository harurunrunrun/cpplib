#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from collections import Counter
from pathlib import Path


def solve(text: str, tokens: list[str], limit: int) -> int:
    multiplicity = Counter(tokens)
    ways = [0] * (len(text) + 1)
    ways[0] = min(1, limit)
    for start in range(len(text)):
        for token, count in multiplicity.items():
            if text.startswith(token, start):
                end = start + len(token)
                ways[end] = min(limit, ways[end] + ways[start] * count)
    return ways[-1]


def encode(value: str) -> str:
    return value if value else "-"


def write_case(
    out_dir: Path,
    index: int,
    cases: list[tuple[str, list[str], int, int]],
) -> None:
    input_lines = [str(len(cases))]
    output_lines = []
    for text, tokens, limit, answer in cases:
        input_lines.append(f"{encode(text)} {len(tokens)} {limit}")
        input_lines.extend(tokens)
        output_lines.append(str(answer))
    stem = f"case_{index:02d}"
    (out_dir / f"{stem}.in").write_text(
        "\n".join(input_lines) + "\n", encoding="ascii"
    )
    (out_dir / f"{stem}.out").write_text(
        "\n".join(output_lines) + "\n", encoding="ascii"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    fixed_inputs = [
        ("", ["a"], 100),
        ("aaaa", ["a", "aa"], 100),
        ("abab", ["ab", "a", "b", "ab"], 1000),
        ("xyz", ["x", "y"], 100),
        ("aaaaaa", ["a", "aa", "aaa"], 7),
    ]
    fixed = [
        (text, tokens, limit, solve(text, tokens, limit))
        for text, tokens, limit in fixed_inputs
    ]
    write_case(out_dir, 0, fixed)

    rng = random.Random(20260717)
    random_cases = []
    for _ in range(250):
        text = "".join(
            rng.choice("abc") for _ in range(rng.randrange(25))
        )
        tokens = [
            "".join(
                rng.choice("abc") for _ in range(rng.randrange(1, 8))
            )
            for _ in range(rng.randrange(1, 16))
        ]
        limit = rng.choice((0, 1, 17, 1_000_000, 2**63 - 1))
        random_cases.append((
            text, tokens, limit, solve(text, tokens, limit)
        ))
    write_case(out_dir, 1, random_cases)

    text_length = 100_000
    token_length = 50_000
    text = "a" * text_length
    token = "a" * token_length + "b"
    write_case(
        out_dir,
        2,
        [(text, [token], 2**63 - 1, 0)],
    )


if __name__ == "__main__":
    main()
