#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from collections import deque
from pathlib import Path

Operation = tuple[int, str | None]


def naive_statistics(text: deque[str]) -> tuple[int, int, int]:
    value = list(text)
    size = len(value)
    palindrome = [[False] * size for _ in range(size)]
    distinct: set[str] = set()
    longest_prefix = 0
    longest_suffix = 0
    for left in range(size - 1, -1, -1):
        for right in range(left, size):
            if value[left] == value[right] and (
                right - left <= 1 or palindrome[left + 1][right - 1]
            ):
                palindrome[left][right] = True
                distinct.add("".join(value[left : right + 1]))
                length = right - left + 1
                if left == 0:
                    longest_prefix = max(longest_prefix, length)
                if right + 1 == size:
                    longest_suffix = max(longest_suffix, length)
    return len(distinct), longest_prefix, longest_suffix


def evaluate(operations: list[Operation]) -> list[tuple[int, int, int]]:
    text: deque[str] = deque()
    answers: list[tuple[int, int, int]] = []
    for operation, character in operations:
        if operation == 0:
            assert character is not None
            text.appendleft(character)
        elif operation == 1:
            assert character is not None
            text.append(character)
        elif operation == 2:
            text.popleft()
        else:
            text.pop()
        answers.append(naive_statistics(text))
    return answers


def write_case(out_dir: Path, name: str, operations: list[Operation]) -> None:
    answers = evaluate(operations)
    input_lines = [str(len(operations))]
    for operation, character in operations:
        if character is None:
            input_lines.append(str(operation))
        else:
            input_lines.append(f"{operation} {character}")
    output_lines = [f"{count} {prefix} {suffix}" for count, prefix, suffix in answers]
    (out_dir / f"{name}.in").write_text(
        "\n".join(input_lines) + "\n", encoding="utf-8"
    )
    (out_dir / f"{name}.out").write_text(
        "\n".join(output_lines) + "\n", encoding="utf-8"
    )


def random_operations(source: random.Random, count: int, limit: int) -> list[Operation]:
    operations: list[Operation] = []
    size = 0
    for _ in range(count):
        if size == 0 or (size < limit and source.randrange(100) < 62):
            operation = source.randrange(2)
            operations.append((operation, chr(ord("a") + source.randrange(5))))
            size += 1
        else:
            operation = 2 + source.randrange(2)
            operations.append((operation, None))
            size -= 1
    return operations


def write_maximum_same_character(out_dir: Path) -> None:
    query_count = 500_000
    input_path = out_dir / "maximum_same_character.in"
    output_path = out_dir / "maximum_same_character.out"
    with input_path.open("w", encoding="utf-8") as input_file, output_path.open(
        "w", encoding="utf-8"
    ) as output_file:
        input_file.write(f"{query_count}\n")
        block_size = 10_000
        for block_begin in range(0, query_count, block_size):
            block_end = min(query_count, block_begin + block_size)
            input_file.write("1 a\n" * (block_end - block_begin))
            output_file.write(
                "".join(f"{length} {length} {length}\n" for length in range(block_begin + 1, block_end + 1))
            )


def write_long_alternating(out_dir: Path) -> None:
    query_count = 120_000
    input_path = out_dir / "long_alternating.in"
    output_path = out_dir / "long_alternating.out"
    with input_path.open("w", encoding="utf-8") as input_file, output_path.open(
        "w", encoding="utf-8"
    ) as output_file:
        input_file.write(f"{query_count}\n")
        for block_begin in range(0, query_count, 10_000):
            block_end = min(query_count, block_begin + 10_000)
            input_file.write(
                "".join(
                    f"1 {'a' if index % 2 == 0 else 'b'}\n"
                    for index in range(block_begin, block_end)
                )
            )
            output_file.write(
                "".join(
                    f"{length} {length if length % 2 == 1 else length - 1} "
                    f"{length if length % 2 == 1 else length - 1}\n"
                    for length in range(block_begin + 1, block_end + 1)
                )
            )


def write_quick_link_adversary(out_dir: Path) -> None:
    pair_count = 125_000
    query_count = pair_count * 4
    input_path = out_dir / "quick_link_adversary.in"
    output_path = out_dir / "quick_link_adversary.out"
    with input_path.open("w", encoding="utf-8") as input_file, output_path.open(
        "w", encoding="utf-8"
    ) as output_file:
        input_file.write(f"{query_count}\n")
        length = 0
        for _ in range(pair_count):
            for character in ("a", "c"):
                input_file.write(f"0 {character}\n")
                length += 1
                longest = length if length % 2 == 1 else length - 1
                output_file.write(f"{length} {longest} {longest}\n")

        prefix = length - 1
        suffix = length - 1
        for index in range(pair_count):
            mode = index % 4
            if mode == 0:
                input_file.write("0 b\n2\n")
                output_file.write(f"{length + 1} 1 {suffix}\n")
            elif mode == 1:
                input_file.write("0 c\n2\n")
                output_file.write(f"{length + 1} 2 {suffix}\n")
            elif mode == 2:
                input_file.write("1 a\n3\n")
                output_file.write(f"{length + 1} {prefix} 2\n")
            else:
                input_file.write("1 c\n3\n")
                output_file.write(f"{length + 1} {length + 1} {length + 1}\n")
            output_file.write(f"{length} {prefix} {suffix}\n")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[tuple[str, list[Operation]]] = [
        ("empty_after_pop", [(1, "a"), (3, None)]),
        (
            "same_character_both_ends",
            [(1, "a")] * 35
            + [(0, "a")] * 35
            + [(2 if index % 2 == 0 else 3, None) for index in range(70)],
        ),
        (
            "alternating_both_ends",
            [(1, "a" if index % 2 == 0 else "b") for index in range(50)]
            + [(0, "b" if index % 2 == 0 else "a") for index in range(30)]
            + [(2 if index % 3 else 3, None) for index in range(80)],
        ),
        (
            "duplicate_and_recreate",
            [(1, character) for character in "abacabadabacaba"]
            + [(2, None), (3, None)] * 7
            + [(0, character) for character in "racecar"]
            + [(1, character) for character in "racecar"]
            + [(3, None)] * 7
            + [(2, None)] * 8,
        ),
    ]

    source = random.Random(2026071329)
    for index in range(24):
        cases.append(
            (
                f"random_{index:02d}",
                random_operations(source, 220 + index * 3, 20 + index % 17),
            )
        )

    for name, operations in cases:
        write_case(args.out_dir, name, operations)
    write_long_alternating(args.out_dir)
    write_quick_link_adversary(args.out_dir)
    write_maximum_same_character(args.out_dir)


if __name__ == "__main__":
    main()
