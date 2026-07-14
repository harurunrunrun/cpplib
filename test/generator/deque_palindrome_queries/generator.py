#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def statistics(text: list[str]) -> tuple[int, int, int]:
    n = len(text)
    palindromes: set[str] = set()
    longest_prefix = 0
    longest_suffix = 0
    joined = "".join(text)
    for left in range(n):
        for right in range(left + 1, n + 1):
            part = joined[left:right]
            if part == part[::-1]:
                palindromes.add(part)
                if left == 0:
                    longest_prefix = max(longest_prefix, right)
                if right == n:
                    longest_suffix = max(longest_suffix, n - left)
    return len(palindromes), longest_prefix, longest_suffix


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(2026071502)
    text: list[str] = []
    operations: list[str] = []
    answers: list[str] = []
    for _ in range(900):
        if not text:
            kind = rng.choice(["PF", "PB"])
        elif len(text) >= 22:
            kind = rng.choice(["OF", "OB"])
        else:
            kind = rng.choice(["PF", "PB", "OF", "OB"])
        if kind in ("PF", "PB"):
            character = rng.choice("abcd")
            operations.append(f"{kind} {character}")
            if kind == "PF":
                text.insert(0, character)
            else:
                text.append(character)
        elif kind == "OF":
            operations.append(kind)
            text.pop(0)
        else:
            operations.append(kind)
            text.pop()
        answers.append(" ".join(map(str, statistics(text))))

    lines = [str(len(operations)), *operations]
    (out_dir / "case_00.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (out_dir / "case_00.out").write_text("\n".join(answers) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
