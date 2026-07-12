#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def encode(value: str) -> str:
    return value if value else "-"


def z_algorithm(value: str) -> list[int]:
    result = [0] * len(value)
    if value:
        result[0] = len(value)
    for index in range(1, len(value)):
        while index + result[index] < len(value) and value[result[index]] == value[index + result[index]]:
            result[index] += 1
    return result


def manacher_odd(value: str) -> list[int]:
    result = [0] * len(value)
    for center in range(len(value)):
        while center - result[center] >= 0 and center + result[center] < len(value) and value[center - result[center]] == value[center + result[center]]:
            result[center] += 1
    return result


def manacher_even(value: str) -> list[int]:
    result = [0] * len(value)
    for center in range(len(value)):
        while center - result[center] - 1 >= 0 and center + result[center] < len(value) and value[center - result[center] - 1] == value[center + result[center]]:
            result[center] += 1
    return result


def suffix_array(value: str) -> list[int]:
    return sorted(range(len(value)), key=lambda index: value[index:])


def lcp_array(value: str, suffixes: list[int]) -> list[int]:
    result: list[int] = []
    for first, second in zip(suffixes, suffixes[1:]):
        length = 0
        while first + length < len(value) and second + length < len(value) and value[first + length] == value[second + length]:
            length += 1
        result.append(length)
    return result


def longest_palindrome_length(value: str) -> int:
    odd = manacher_odd(value)
    even = manacher_even(value)
    return max([0, *(2 * radius - 1 for radius in odd), *(2 * radius for radius in even)])


def longest_common_substring_length(a: str, b: str) -> int:
    best = 0
    for i in range(len(a)):
        for j in range(len(b)):
            length = 0
            while i + length < len(a) and j + length < len(b) and a[i + length] == b[j + length]:
                length += 1
            best = max(best, length)
    return best


def format_vector(values: list[int]) -> str:
    return " ".join(map(str, [len(values), *values]))


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(20260713)
    cases = [
        ("", ""),
        ("a", "b"),
        ("aabcaabxaaaz", "aabx"),
        ("banana", "ananas"),
        ("a" * 60, "a" * 55),
        ("abcddcba", "zzabcddcbayy"),
    ]
    for _ in range(120):
        a = "".join(rng.choice("abcd") for _ in range(rng.randrange(61)))
        b = "".join(rng.choice("abcd") for _ in range(rng.randrange(61)))
        cases.append((a, b))

    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for a, b in cases:
        input_lines.append(f"{encode(a)} {encode(b)}")
        suffixes = suffix_array(a)
        output_lines.extend(
            [
                format_vector(z_algorithm(a)),
                format_vector(manacher_odd(a)),
                format_vector(manacher_even(a)),
                format_vector(suffixes),
                format_vector(lcp_array(a, suffixes)),
                f"{longest_palindrome_length(a)} {longest_common_substring_length(a, b)}",
            ]
        )

    (out_dir / "case_00.in").write_text("\n".join(input_lines) + "\n", encoding="utf-8")
    (out_dir / "case_00.out").write_text("\n".join(output_lines) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
