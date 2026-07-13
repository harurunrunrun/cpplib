#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def occurrences(text: str, pattern: str) -> list[int]:
    return [
        position
        for position in range(len(text) - len(pattern) + 1)
        if text.startswith(pattern, position)
    ]


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(20260713)
    cases = [
        ("a" * 50000, "a" * 137),
        ("".join(rng.choice("abcd") for _ in range(60000)), "abacabadabacaba"),
        ("abc", "z"),
    ]
    for case_id, (text, pattern) in enumerate(cases):
        answer = occurrences(text, pattern)
        (out_dir / f"case_{case_id:02d}.in").write_text(
            f"{text}\n{pattern}\n", encoding="utf-8"
        )
        (out_dir / f"case_{case_id:02d}.out").write_text(
            "".join(f"{position}\n" for position in answer), encoding="utf-8"
        )


if __name__ == "__main__":
    main()
