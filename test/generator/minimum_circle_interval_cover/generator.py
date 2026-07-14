#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import itertools
import random
from pathlib import Path


def mask_of(size: int, start: int, end: int) -> int:
    if abs(end - start) >= size:
        return (1 << size) - 1
    length = (end - start) % size
    result = 0
    for offset in range(length):
        result |= 1 << ((start + offset) % size)
    return result


def brute(size: int, intervals: list[tuple[int, int]]) -> int:
    masks = [mask_of(size, start, end) for start, end in intervals]
    target = (1 << size) - 1
    for count in range(1, len(masks) + 1):
        for chosen in itertools.combinations(masks, count):
            union = 0
            for mask in chosen:
                union |= mask
            if union == target:
                return count
    return -1


def write_cases(
    output_directory: Path,
    index: int,
    cases: list[tuple[int, list[tuple[int, int]], int]],
) -> None:
    with (output_directory / f"case_{index:02d}.in").open(
        "w", encoding="utf-8", newline="\n"
    ) as input_file, (output_directory / f"case_{index:02d}.out").open(
        "w", encoding="utf-8", newline="\n"
    ) as output_file:
        input_file.write(f"{len(cases)}\n")
        for size, intervals, answer in cases:
            input_file.write(f"{len(intervals)} {size}\n")
            input_file.writelines(f"{start} {end}\n"
                                  for start, end in intervals)
            output_file.write(f"{answer}\n")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    arguments = parser.parse_args()
    arguments.out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[tuple[int, list[tuple[int, int]], int]] = [
        (12, [], -1),
        (12, [(0, 0)], -1),
        (12, [(3, 15)], 1),
        (12, [(9, 3), (3, 9)], 2),
        (12, [(0, 4), (4, 8), (8, 12)], 3),
        (12, [(0, 5), (4, 9), (8, 1)], 3),
        (12, [(0, 5), (6, 11)], -1),
    ]
    source = random.Random(2026071505)
    for _ in range(120):
        size = 18
        count = source.randint(0, 11)
        intervals = [
            (source.randrange(size), source.randrange(size))
            for _ in range(count)
        ]
        if source.randrange(10) == 0:
            start = source.randrange(size)
            intervals.append((start, start + size))
        cases.append((size, intervals, brute(size, intervals)))
    write_cases(arguments.out_dir, 0, cases)

    size = 200000
    intervals = [
        (0, 50000),
        (50000, 100000),
        (100000, 150000),
        (150000, 200000),
    ]
    intervals.extend((0, 1 + index % 49999) for index in range(199996))
    write_cases(arguments.out_dir, 1, [(size, intervals, 4)])


if __name__ == "__main__":
    main()
