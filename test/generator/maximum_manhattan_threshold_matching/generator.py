#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from functools import lru_cache
from pathlib import Path

Point = tuple[int, int]
Case = tuple[list[Point], list[Point], int]


def maximum_matching(
    left_points: list[Point],
    right_points: list[Point],
    maximum_distance: int,
) -> int:
    adjacent = [
        [
            right
            for right, (right_x, right_y) in enumerate(right_points)
            if abs(left_x - right_x) + abs(left_y - right_y)
            <= maximum_distance
        ]
        for left_x, left_y in left_points
    ]

    @lru_cache(maxsize=None)
    def solve(left: int, used_right: int) -> int:
        if left == len(left_points):
            return 0
        answer = solve(left + 1, used_right)
        for right in adjacent[left]:
            if (used_right >> right) & 1:
                continue
            answer = max(
                answer,
                1 + solve(left + 1, used_right | (1 << right)),
            )
        return answer

    return solve(0, 0)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    low = -(1 << 63)
    high = (1 << 63) - 1
    cases: list[Case] = [
        ([], [], 0),
        ([(0, 0)], [], 0),
        ([(0, 0), (0, 0)], [(0, 0), (0, 0)], 0),
        ([(-5, 7), (4, -2)], [(1, 3), (20, 20)], 8),
        ([(low, 0)], [(high, 0)], high),
    ]

    source = random.Random(41001)
    for _ in range(160):
        left_size = source.randint(0, 8)
        right_size = source.randint(0, 8)
        left_points = [
            (source.randint(-20, 20), source.randint(-20, 20))
            for _ in range(left_size)
        ]
        right_points = [
            (source.randint(-20, 20), source.randint(-20, 20))
            for _ in range(right_size)
        ]
        cases.append((
            left_points,
            right_points,
            source.randint(0, 50),
        ))

    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for left_points, right_points, maximum_distance in cases:
        input_lines.append(
            f"{len(left_points)} {len(right_points)} {maximum_distance}"
        )
        input_lines.extend(f"{x} {y}" for x, y in left_points)
        input_lines.extend(f"{x} {y}" for x, y in right_points)
        output_lines.append(str(maximum_matching(
            left_points,
            right_points,
            maximum_distance,
        )))

    (out_dir / "case_00.in").write_text(
        "\n".join(input_lines) + "\n",
        encoding="ascii",
    )
    (out_dir / "case_00.out").write_text(
        "\n".join(output_lines) + "\n",
        encoding="ascii",
    )


if __name__ == "__main__":
    main()
