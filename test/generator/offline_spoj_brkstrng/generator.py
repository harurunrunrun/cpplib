#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


Case = tuple[int, list[int], int | None]


def solve_naive(interval_length: int, cut_positions: list[int]) -> int:
    """Independent cubic interval DP used as the oracle for ordinary cases."""

    positions = [0, *cut_positions, interval_length]
    size = len(positions)
    dp = [[0] * size for _ in range(size)]
    for span in range(2, size):
        for left in range(size - span):
            right = left + span
            dp[left][right] = positions[right] - positions[left] + min(
                dp[left][cut] + dp[cut][right]
                for cut in range(left + 1, right)
            )
    return dp[0][-1]


def balanced_unit_segment_cost(segment_count: int) -> int:
    """Optimal external path length for equal-width unit segments."""

    height = (segment_count - 1).bit_length()
    return segment_count * height - ((1 << height) - segment_count)


def write_suite(out_dir: Path, name: str, cases: list[Case]) -> None:
    input_lines: list[str] = []
    output_lines: list[str] = []
    for interval_length, cut_positions, expected in cases:
        assert interval_length > 0
        assert cut_positions == sorted(set(cut_positions))
        assert all(0 < position < interval_length for position in cut_positions)
        input_lines.append(f"{interval_length} {len(cut_positions)}")
        input_lines.append(" ".join(map(str, cut_positions)))
        if expected is None:
            expected = solve_naive(interval_length, cut_positions)
        output_lines.append(str(expected))
    (out_dir / f"{name}.in").write_text(
        "\n".join(input_lines) + "\n", encoding="ascii"
    )
    (out_dir / f"{name}.out").write_text(
        "\n".join(output_lines) + "\n", encoding="ascii"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    # The two closed forms used by the maximum-like cases are checked against
    # the independent cubic oracle on smaller instances.
    for segment_count in range(1, 33):
        equal_cuts = list(range(1, segment_count))
        assert solve_naive(segment_count, equal_cuts) == (
            balanced_unit_segment_cost(segment_count)
        )
        long_tail_length = segment_count + 37
        long_tail_cuts = list(range(1, segment_count + 1))
        assert solve_naive(long_tail_length, long_tail_cuts) == (
            long_tail_length + balanced_unit_segment_cost(segment_count)
        )

    write_suite(
        out_dir,
        "boundary",
        [
            (1, [], None),
            (2, [], None),
            (2, [1], None),
            (3, [1, 2], None),
            (10, [1, 9], None),
            (20, [3, 8, 10], None),
            (20, [2, 3, 8, 10], None),
            (1_000_000_000, [], None),
            (1_000_000_000, [1], None),
            (1_000_000_000, [1, 999_999_999], None),
        ],
    )

    exhaustive: list[Case] = [(1, [], None)]
    for interval_length in range(2, 12):
        for mask in range(1 << (interval_length - 1)):
            cuts = [
                position
                for position in range(1, interval_length)
                if mask >> (position - 1) & 1
            ]
            exhaustive.append((interval_length, cuts, None))
    write_suite(out_dir, "exhaustive_small", exhaustive)

    rng = random.Random(0x42524B5354524E47)
    random_cases: list[Case] = []
    for _ in range(300):
        interval_length = rng.randint(2, 300)
        cut_count = rng.randint(0, min(24, interval_length - 1))
        cuts = sorted(rng.sample(range(1, interval_length), cut_count))
        random_cases.append((interval_length, cuts, None))
    for cut_count in (64, 96, 128, 160):
        interval_length = 10_000
        cuts = sorted(rng.sample(range(1, interval_length), cut_count))
        random_cases.append((interval_length, cuts, None))
    write_suite(out_dir, "random_naive", random_cases)

    maximum_cut_count = 1_000
    long_tail_length = 10_000_000
    long_tail_cuts = list(range(1, maximum_cut_count + 1))
    long_tail_answer = long_tail_length + balanced_unit_segment_cost(
        maximum_cut_count
    )

    equal_segment_count = 1_000
    equal_segment_width = 1_000
    equal_interval_length = equal_segment_count * equal_segment_width
    equal_cuts = [
        equal_segment_width * index
        for index in range(1, equal_segment_count)
    ]
    equal_answer = (
        equal_segment_width * balanced_unit_segment_cost(equal_segment_count)
    )
    write_suite(
        out_dir,
        "maximum_like",
        [
            (long_tail_length, long_tail_cuts, long_tail_answer),
            (equal_interval_length, equal_cuts, equal_answer),
        ],
    )


if __name__ == "__main__":
    main()
