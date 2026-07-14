#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def solve_brute(text: str, length: int) -> int:
    if length <= 0:
        return 0
    return sum(
        text[start:start + length] == text[start:start + length][::-1]
        for start in range(len(text) - length + 1)
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[tuple[int, str]] = [
        (1, "a"),
        (2, "aaaa"),
        (3, "ababa"),
        (5, "ababab"),
        (7, "abc"),
    ]
    rng = random.Random(30000095)
    for _ in range(400):
        text = "".join(rng.choice("abcABC") for _ in range(rng.randint(1, 70)))
        length = rng.randint(1, len(text) + 3)
        cases.append((length, text))

    (out_dir / "case_00.in").write_text(
        "\n".join([str(len(cases)), *(f"{length} {text}" for length, text in cases)]) + "\n",
        encoding="ascii",
    )
    (out_dir / "case_00.out").write_text(
        "\n".join(str(solve_brute(text, length)) for length, text in cases) + "\n",
        encoding="ascii",
    )


if __name__ == "__main__":
    main()
