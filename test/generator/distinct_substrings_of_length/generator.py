#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def solve_brute(text: str, length: int) -> int:
    if length <= 0:
        return 0
    return len({text[start:start + length] for start in range(len(text) - length + 1)})


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[tuple[int, str]] = [
        (1, "aaa"),
        (2, "aaa"),
        (2, "abac"),
        (0, "abc"),
        (4, "abc"),
    ]
    rng = random.Random(300000100)
    for _ in range(400):
        text = "".join(rng.choice("abcde") for _ in range(rng.randint(1, 80)))
        cases.append((rng.randint(1, len(text) + 3), text))

    (out_dir / "case_00.in").write_text(
        "\n".join([str(len(cases)), *(f"{length} {text}" for length, text in cases)]) + "\n",
        encoding="ascii",
    )
    (out_dir / "case_00.out").write_text(
        "\n".join(str(solve_brute(text, length)) for length, text in cases) + "\n",
        encoding="ascii",
    )
    size = 300_000
    alphabet = "abcdefghijklmnopqrstuvwxyz"
    periodic = (alphabet * (size // len(alphabet) + 1))[:size]
    large_cases = [(size // 2, "a" * size), (size // 2, periodic)]
    (out_dir / "case_01.in").write_text(
        "\n".join(
            ["2", *(f"{length} {text}" for length, text in large_cases)]
        ) + "\n",
        encoding="ascii",
    )
    (out_dir / "case_01.out").write_text("1\n26\n", encoding="ascii")



if __name__ == "__main__":
    main()
