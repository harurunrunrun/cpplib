#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def is_palindrome(s: str) -> bool:
    return s == s[::-1]


def palindromes(s: str) -> set[str]:
    result: set[str] = set()
    for left in range(len(s)):
        for right in range(left + 1, len(s) + 1):
            t = s[left:right]
            if is_palindrome(t):
                result.add(t)
    return result


def count_pal_substrings(s: str) -> int:
    result = 0
    for left in range(len(s)):
        for right in range(left + 1, len(s) + 1):
            result += int(is_palindrome(s[left:right]))
    return result


def occurrence(s: str, t: str) -> int:
    return sum(1 for i in range(len(s) - len(t) + 1) if s[i:i + len(t)] == t)


def suffix_lengths(s: str) -> list[int]:
    return [length for length in range(len(s), 0, -1) if is_palindrome(s[-length:])]


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()

    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(20260725)
    cases = ["ababa", "aaaa", "abacaba"]
    for _ in range(25):
        n = rng.randrange(1, 31)
        cases.append("".join(chr(ord("a") + rng.randrange(4)) for _ in range(n)))

    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for s in cases:
        pals = sorted(palindromes(s))
        queries = sorted(set(pals[:4] + pals[-4:] + ["a", "b", "ab", "ba", "abc"]))
        input_lines.append(f"{s} {len(queries)}")
        input_lines.extend(queries)

        suffix = suffix_lengths(s)
        output_lines.append(
            f"{len(pals)} {count_pal_substrings(s)} "
            f"{max(map(len, pals), default=0)} {max(suffix, default=0)}"
        )
        output_lines.append(" ".join(pals))
        output_lines.append(" ".join(map(str, suffix)))
        for t in queries:
            output_lines.append(f"{int(t in pals)} {occurrence(s, t) if t in pals else 0}")

    (out_dir / "case_00.in").write_text("\n".join(input_lines) + "\n", encoding="utf-8")
    (out_dir / "case_00.out").write_text("\n".join(output_lines) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
