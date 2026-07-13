#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def matches_naive(text: str, pattern: str) -> str:
    result: list[str] = []
    for start in range(len(text) - len(pattern) + 1):
        matched = all(
            text[start + index] == "*"
            or pattern[index] == "*"
            or text[start + index] == pattern[index]
            for index in range(len(pattern))
        )
        result.append("1" if matched else "0")
    return "".join(result)


def first_mod_collision(random_source: random.Random) -> tuple[str, str]:
    modulus = 998_244_353
    size = 1 << 18
    half = size // 2
    alphabet = "abcdefghijklmnopqrstuvwxyz*"
    text = ["*"] * size
    pattern = ["*"] * size

    def score(index: int) -> int:
        left = 0 if text[index] == "*" else ord(text[index]) + 1
        right = 0 if pattern[index] == "*" else ord(pattern[index]) + 1
        return left * right * (left - right) ** 2

    prefix_position: dict[int, int] = {}
    prefix_sum = 0
    for index in range(half):
        text[index] = random_source.choice(alphabet)
        pattern[index] = random_source.choice(alphabet)
        prefix_sum += score(index)
        prefix_position[prefix_sum % modulus] = index + 1

    suffix_sum = 0
    while True:
        index = random_source.randrange(half, size)
        suffix_sum -= score(index)
        text[index] = random_source.choice(alphabet)
        pattern[index] = random_source.choice(alphabet)
        suffix_sum += score(index)
        cutoff = prefix_position.get((-suffix_sum) % modulus)
        if suffix_sum != 0 and cutoff is not None:
            for position in range(cutoff, half):
                text[position] = "*"
                pattern[position] = "*"
            break

    total = sum(score(index) for index in range(size))
    assert total != 0
    assert total % modulus == 0
    return "".join(text), "".join(pattern)


def write_case(
    out_dir: Path,
    index: int,
    text: str,
    pattern: str,
    answer: str | None = None,
) -> None:
    if answer is None:
        answer = matches_naive(text, pattern)
    (out_dir / f"case_{index:02d}.in").write_text(
        f"{text}\n{pattern}\n", encoding="utf-8"
    )
    (out_dir / f"case_{index:02d}.out").write_text(
        answer + "\n", encoding="utf-8"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)

    write_case(args.out_dir, 0, "a", "a")
    write_case(args.out_dir, 1, "a", "b")
    write_case(args.out_dir, 2, "abcabc", "a*c")
    write_case(args.out_dir, 3, "***", "ab")
    write_case(args.out_dir, 4, "abababab", "***")
    write_case(args.out_dir, 5, "abcde", "abcde")

    random_source = random.Random(2026071314)
    alphabet = "abcde*"
    index = 6
    for _ in range(60):
        text_size = random_source.randint(1, 80)
        pattern_size = random_source.randint(1, text_size)
        text = "".join(random_source.choice(alphabet) for _ in range(text_size))
        pattern = "".join(
            random_source.choice(alphabet) for _ in range(pattern_size)
        )
        write_case(args.out_dir, index, text, pattern)
        index += 1

    size = 524_288
    block = "abcdefghijklmnopqrstuvwxyz*"
    maximum = (block * ((size + len(block) - 1) // len(block)))[:size]
    write_case(args.out_dir, index, maximum, maximum, "1")
    index += 1

    text = ("ab*" * ((size + 2) // 3))[:size]
    answer = "".join("1" if value in "a*" else "0" for value in text)
    write_case(args.out_dir, index, text, "a", answer)
    index += 1

    collision_text, collision_pattern = first_mod_collision(random_source)
    write_case(args.out_dir, index, collision_text, collision_pattern, "0")


if __name__ == "__main__":
    main()
