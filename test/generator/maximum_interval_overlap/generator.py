#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


Interval = tuple[int, int]


def solve_brute(intervals: list[Interval], closed: bool) -> int:
    if not intervals:
        return 0
    candidates = sorted({coordinate for interval in intervals for coordinate in interval})
    answer = 0
    for coordinate in candidates:
        answer = max(
            answer,
            sum(
                left <= coordinate <= right if closed else left <= coordinate < right
                for left, right in intervals
            ),
        )
    return answer


def write_case(
    out_dir: Path,
    index: int,
    intervals: list[Interval],
    closed: bool,
) -> None:
    name = f"case_{index:03d}"
    lines = [f"{int(closed)} {len(intervals)}"]
    lines.extend(f"{left} {right}" for left, right in intervals)
    (out_dir / f"{name}.in").write_text("\n".join(lines) + "\n", encoding="ascii")
    (out_dir / f"{name}.out").write_text(
        f"{solve_brute(intervals, closed)}\n",
        encoding="ascii",
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[tuple[list[Interval], bool]] = [
        ([], False),
        ([(1, 1)], False),
        ([(1, 1)], True),
        ([(1, 2), (2, 3)], False),
        ([(1, 2), (2, 3)], True),
        ([(1, 7), (2, 4), (6, 9), (3, 8), (5, 10)], False),
    ]
    rng = random.Random(100000114)
    for _ in range(160):
        intervals: list[Interval] = []
        for _ in range(rng.randint(0, 30)):
            left = rng.randint(-30, 30)
            right = rng.randint(left, 30)
            intervals.append((left, right))
        cases.append((intervals, bool(rng.randrange(2))))
    for index, (intervals, closed) in enumerate(cases):
        write_case(out_dir, index, intervals, closed)


if __name__ == "__main__":
    main()
