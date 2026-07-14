#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import itertools
import random
from pathlib import Path


def brute_answer(values: list[int], partition_count: int) -> int:
    result: int | None = None
    for cuts in itertools.combinations(range(1, len(values)), partition_count - 1):
        boundaries = (0, *cuts, len(values))
        maximum = max(
            sum(values[boundaries[index] : boundaries[index + 1]])
            for index in range(partition_count)
        )
        result = maximum if result is None else min(result, maximum)
    assert result is not None
    return result


def write_case(
    out_dir: Path,
    index: int,
    values: list[int],
    partition_count: int,
    expected: int | None = None,
) -> None:
    answer = brute_answer(values, partition_count) if expected is None else expected
    (out_dir / f"case_{index:02d}.in").write_text(
        f"{len(values)} {partition_count}\n"
        + " ".join(map(str, values))
        + "\n",
        encoding="utf-8",
    )
    (out_dir / f"case_{index:02d}.out").write_text(
        f"{answer}\n", encoding="utf-8"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[tuple[list[int], int]] = [
        ([1, 1, 1, 3, 2, 2, 1, 3, 1], 4),
        ([10, -9, -9, 10], 2),
        ([-5, -2, -7], 2),
        ([4, -20, 7, 8, -3], 3),
        ([7], 1),
        ([3, -10, 4], 1),
        ([3, -10, 4], 3),
        ([30000, -30000, 30000, -30000, 30000], 2),
        ([-30000, 30000, -30000, 30000, -30000], 4),
        ([10**18, -10**18, 10**18, -10**18], 3),
    ]

    random_generator = random.Random(2026071407)
    for _ in range(40):
        size = random_generator.randrange(1, 12)
        partition_count = random_generator.randrange(1, size + 1)
        values = [random_generator.randrange(-50, 51) for _ in range(size)]
        cases.append((values, partition_count))

    for index, (values, partition_count) in enumerate(cases):
        write_case(args.out_dir, index, values, partition_count)

    large_size = 15000
    write_case(
        args.out_dir,
        len(cases),
        [30000 if index % 2 == 0 else -30000 for index in range(large_size)],
        large_size // 2,
        0,
    )


if __name__ == "__main__":
    main()
