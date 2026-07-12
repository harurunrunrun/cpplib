#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def distinct_substrings(s: str) -> int:
    return len({s[left:right] for left in range(len(s)) for right in range(left + 1, len(s) + 1)})


def longest_common_substring(a: str, b: str) -> int:
    best = 0
    for left in range(len(b)):
        for right in range(left + 1, len(b) + 1):
            if right - left > best and b[left:right] in a:
                best = right - left
    return best


def occurrence_count(s: str, pattern: str) -> int:
    width = len(pattern)
    return sum(s[index:index + width] == pattern for index in range(len(s) - width + 1))


def write_case(out_dir: Path, name: str, cases: list[tuple[str, str, list[str]]]) -> None:
    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for source, target, patterns in cases:
        input_lines.append(f"{source} {target} {len(patterns)}")
        input_lines.extend(patterns)
        output_lines.append(f"{distinct_substrings(source)} {longest_common_substring(source, target)}")
        for pattern in patterns:
            output_lines.append(f"{int(pattern in source)} {occurrence_count(source, pattern)}")

    (out_dir / f"{name}.in").write_text("\n".join(input_lines) + "\n")
    (out_dir / f"{name}.out").write_text("\n".join(output_lines) + "\n")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    write_case(
        out_dir,
        "hand",
        [
            ("a", "a", ["a", "aa", "b"]),
            ("ababa", "babab", ["a", "aba", "ba", "ababa", "bb"]),
            ("aaaaaaaa", "baaaaab", ["a", "aa", "aaaa", "aaaaaaaa", "aaaaaaaaa"]),
            ("abcdabcd", "dcbaabcd", ["abcd", "bcd", "dabc", "ac", "cdabcd"]),
        ],
    )

    for seed in range(4):
        rng = random.Random(20260724 + seed)
        cases: list[tuple[str, str, list[str]]] = []
        for _ in range(18):
            source = "".join(rng.choice("abcd") for _ in range(rng.randint(1, 45)))
            target = "".join(rng.choice("abcd") for _ in range(rng.randint(1, 45)))
            patterns: list[str] = []
            for _ in range(18):
                if rng.randrange(2):
                    left = rng.randrange(len(source))
                    right = rng.randrange(left + 1, len(source) + 1)
                    patterns.append(source[left:right])
                else:
                    patterns.append("".join(rng.choice("abcd") for _ in range(rng.randint(1, 12))))
            cases.append((source, target, patterns))
        write_case(out_dir, f"random_{seed:02d}", cases)


if __name__ == "__main__":
    main()