#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


MAX_TEST_COUNT = 10
MAX_LENGTH = 1_000_000


def periodic_prefixes_brute(text: str) -> list[tuple[int, int]]:
    result = []
    for prefix_length in range(2, len(text) + 1):
        prefix = text[:prefix_length]
        for block_length in range(1, prefix_length):
            if prefix_length % block_length != 0:
                continue
            repetition_count = prefix_length // block_length
            if prefix == prefix[:block_length] * repetition_count:
                result.append((prefix_length, repetition_count))
                break
    return result


def validate(texts: list[str]) -> None:
    assert 1 <= len(texts) <= MAX_TEST_COUNT
    assert all(2 <= len(text) <= MAX_LENGTH for text in texts)
    assert all(all(97 <= ord(character) <= 126 for character in text) for text in texts)


def make_input(texts: list[str]) -> str:
    lines = [str(len(texts))]
    for text in texts:
        lines.extend((str(len(text)), text))
    return "\n".join(lines) + "\n"


def make_output(results: list[list[tuple[int, int]]]) -> str:
    lines: list[str] = []
    for test_case, pairs in enumerate(results, 1):
        lines.append(f"Test case #{test_case}")
        lines.extend(f"{length} {count}" for length, count in pairs)
        lines.append("")
    return "\n".join(lines) + "\n"


def write_small_case(out_dir: Path, name: str, texts: list[str]) -> None:
    validate(texts)
    results = [periodic_prefixes_brute(text) for text in texts]
    (out_dir / f"{name}.in").write_text(make_input(texts), encoding="ascii")
    (out_dir / f"{name}.out").write_text(make_output(results), encoding="ascii")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    write_small_case(out_dir, "official", ["aaa", "aabaabaabaab"])
    write_small_case(
        out_dir,
        "fixed",
        ["aa", "ab", "abab", "abcabcabc", "aaaaab", "{~{~", "~~~~~~"],
    )

    source = random.Random(25030088)
    alphabet = "abc{~"
    for case_index in range(10):
        texts = [
            "".join(source.choice(alphabet) for _ in range(source.randint(2, 45)))
            for _ in range(MAX_TEST_COUNT)
        ]
        write_small_case(out_dir, f"random_{case_index:02d}", texts)

    maximum_cases = [
        "aa",
        "ab",
        "aaa",
        "abab",
        "abcabc",
        "~" * 7,
        "{~" * 8,
        "aabaabaabaab",
        "abcdefghij",
        "z" * 32,
    ]
    write_small_case(out_dir, "max_test_count", maximum_cases)

    all_equal = "a" * MAX_LENGTH
    (out_dir / "max_length_and_output.in").write_text(
        make_input([all_equal]),
        encoding="ascii",
    )
    with (out_dir / "max_length_and_output.out").open("w", encoding="ascii") as output:
        output.write("Test case #1\n")
        for prefix_length in range(2, MAX_LENGTH + 1):
            output.write(f"{prefix_length} {prefix_length}\n")
        output.write("\n")

    no_period = "a" + "b" * (MAX_LENGTH - 1)
    (out_dir / "max_length_no_period.in").write_text(
        make_input([no_period]),
        encoding="ascii",
    )
    (out_dir / "max_length_no_period.out").write_text(
        "Test case #1\n\n",
        encoding="ascii",
    )


if __name__ == "__main__":
    main()
