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


def longest_suffix(s: str) -> int:
    for length in range(len(s), 0, -1):
        if is_palindrome(s[-length:]):
            return length
    return 0


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()

    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(20261114)
    cases = ["ababa", "aaaa", "abacaba"]
    for _ in range(20):
        n = rng.randrange(1, 45)
        cases.append("".join(chr(ord("a") + rng.randrange(4)) for _ in range(n)))

    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for s in cases:
        queries: list[tuple[int, str]] = []
        full_pals = sorted(palindromes(s))
        for t in full_pals[:4] + full_pals[-4:] + ["a", "b", "ab", "ba", "abcd"]:
            version = rng.randrange(0, len(s) + 1)
            queries.append((version, t))
        input_lines.append(f"{s} {len(queries)}")
        for version, t in queries:
            input_lines.append(f"{version} {t}")

        for version in range(len(s) + 1):
            prefix = s[:version]
            pals = palindromes(prefix)
            output_lines.append(
                f"{len(pals)} {count_pal_substrings(prefix)} "
                f"{max(map(len, pals), default=0)} {longest_suffix(prefix)}"
            )
        for version, t in queries:
            output_lines.append(str(int(t in palindromes(s[:version]))))

    (out_dir / "case_00.in").write_text("\n".join(input_lines) + "\n", encoding="utf-8")
    (out_dir / "case_00.out").write_text("\n".join(output_lines) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
