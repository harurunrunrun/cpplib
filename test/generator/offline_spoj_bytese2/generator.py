#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


MAX_TEST_COUNT = 100
MAX_INTERVAL_COUNT = 100
MAX_TIME = 10_000_000
Interval = tuple[int, int]


def solve_brute(intervals: list[Interval]) -> int:
    return max(
        sum(left <= time < right for left, right in intervals)
        for time in (left for left, _ in intervals)
    )


def write_suite(
    out_dir: Path,
    name: str,
    cases: list[list[Interval]],
    *,
    final_newline: bool = True,
) -> None:
    assert 1 <= len(cases) <= MAX_TEST_COUNT
    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for intervals in cases:
        assert 1 <= len(intervals) <= MAX_INTERVAL_COUNT
        assert all(0 <= left < right <= MAX_TIME for left, right in intervals)
        input_lines.append(str(len(intervals)))
        input_lines.extend(f"{left} {right}" for left, right in intervals)
        output_lines.append(str(solve_brute(intervals)))
    (out_dir / f"{name}.in").write_text(
        "\n".join(input_lines) + ("\n" if final_newline else ""),
        encoding="ascii",
    )
    (out_dir / f"{name}.out").write_text(
        "\n".join(output_lines) + "\n",
        encoding="ascii",
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    write_suite(
        out_dir,
        "official",
        [[(1, 7), (2, 4), (6, 9), (3, 8), (5, 10)]],
    )

    rng = random.Random(100000114)
    random_cases: list[list[Interval]] = []
    for _ in range(240):
        interval_count = rng.randint(1, MAX_INTERVAL_COUNT)
        endpoints = rng.sample(range(0, 5000), interval_count * 2)
        intervals = []
        for index in range(interval_count):
            left, right = sorted((endpoints[index * 2], endpoints[index * 2 + 1]))
            intervals.append((left, right))
        random_cases.append(intervals)
    for offset in range(0, len(random_cases), MAX_TEST_COUNT):
        write_suite(
            out_dir,
            f"random_{offset // MAX_TEST_COUNT:02d}",
            random_cases[offset:offset + MAX_TEST_COUNT],
        )

    maximum_cases: list[list[Interval]] = []
    for test_case in range(MAX_TEST_COUNT):
        base = test_case * 200
        maximum_cases.append([
            (base + interval, MAX_TIME - base - interval)
            for interval in range(MAX_INTERVAL_COUNT)
        ])
    write_suite(
        out_dir,
        "maximum_constraints",
        maximum_cases,
        final_newline=False,
    )


if __name__ == "__main__":
    main()
