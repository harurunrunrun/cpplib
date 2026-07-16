#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def naive_counts(intervals: list[tuple[int, int]]) -> list[int]:
    prefix: list[tuple[int, int]] = []
    answers: list[int] = []
    for interval in intervals:
        prefix.append(interval)
        previous_finish: int | None = None
        count = 0
        for start, finish in sorted(prefix, key=lambda item: (item[1], -item[0])):
            if previous_finish is None or previous_finish <= start:
                previous_finish = finish
                count += 1
        answers.append(count)
    return answers


def write_case(
    directory: Path,
    index: int,
    minimum_coordinate: int,
    maximum_coordinate: int,
    intervals: list[tuple[int, int]],
    answers: list[int],
) -> None:
    assert len(intervals) == len(answers)
    input_lines = [
        f"{minimum_coordinate} {maximum_coordinate} {len(intervals)}",
        *(f"{start} {finish}" for start, finish in intervals),
    ]
    (directory / f"case_{index:02d}.in").write_text(
        "\n".join(input_lines) + "\n", encoding="utf-8"
    )
    (directory / f"case_{index:02d}.out").write_text(
        "\n".join(map(str, answers)) + "\n", encoding="utf-8"
    )


def random_case(seed: int, count: int) -> list[tuple[int, int]]:
    random_engine = random.Random(seed)
    intervals: list[tuple[int, int]] = []
    for _ in range(count):
        start = random_engine.randrange(-50, 50)
        finish = random_engine.randrange(start + 1, 51)
        intervals.append((start, finish))
    return intervals


def large_case() -> tuple[list[tuple[int, int]], list[int]]:
    half = 60000
    intervals: list[tuple[int, int]] = []
    answers: list[int] = []
    for index in range(half):
        intervals.append((index, 120001 - index))
        answers.append(1)
    for index in range(half):
        start = 120002 + index * 2
        intervals.append((start, start + 1))
        answers.append(index + 2)
    return intervals, answers


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    arguments = parser.parse_args()
    directory = Path(arguments.out_dir)
    directory.mkdir(parents=True, exist_ok=True)

    boundary = [
        (0, 20),
        (2, 18),
        (4, 16),
        (4, 16),
        (16, 17),
        (1, 19),
        (5, 6),
        (6, 7),
        (3, 15),
        (7, 9),
        (9, 12),
    ]
    write_case(directory, 0, 0, 20, boundary, naive_counts(boundary))

    random_intervals = random_case(2026071612, 1200)
    write_case(
        directory,
        1,
        -50,
        51,
        random_intervals,
        naive_counts(random_intervals),
    )

    intervals, answers = large_case()
    write_case(directory, 2, 0, 240003, intervals, answers)


if __name__ == "__main__":
    main()
