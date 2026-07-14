#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import bisect
import random
from pathlib import Path


def answers(targets: list[int], increments: list[int]) -> list[int]:
    prefix = [0]
    for value in increments:
        prefix.append(prefix[-1] + value)
    return [
        position if position != len(prefix) else len(increments) + 1
        for target in targets
        for position in [bisect.bisect_left(prefix, target)]
    ]


def write_case(
    out_dir: Path,
    index: int,
    targets: list[int],
    increments: list[int],
) -> None:
    input_lines = [
        f"{len(targets)} {len(increments)}",
        " ".join(map(str, targets)),
        " ".join(map(str, increments)),
    ]
    (out_dir / f"case_{index:02d}.in").write_text(
        "\n".join(input_lines) + "\n", encoding="utf-8"
    )
    (out_dir / f"case_{index:02d}.out").write_text(
        " ".join(map(str, answers(targets, increments))) + "\n",
        encoding="utf-8",
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[tuple[list[int], list[int]]] = [
        ([], []),
        ([], [1, 2, 3]),
        ([-10, 0, 1, 10], []),
        ([-5, 0, 1, 3, 6, 7], [1, 2, 3]),
        ([0, 1, 2, 3, 4, 5], [0, 0, 5, 0]),
        ([10**18 - 1, 10**18, 10**18 + 1], [10**18]),
        ([0, 1, 6, 7, 100], [1, 2, 3]),
    ]

    random_generator = random.Random(2026071402)
    for _ in range(30):
        operation_count = random_generator.randrange(0, 90)
        increments = [
            random_generator.randrange(0, 10**6)
            for _ in range(operation_count)
        ]
        total = sum(increments)
        query_count = random_generator.randrange(0, 90)
        targets = [
            random_generator.randrange(-10**6, total + 10**6 + 1)
            for _ in range(query_count)
        ]
        cases.append((targets, increments))

    for index, (targets, increments) in enumerate(cases):
        write_case(args.out_dir, index, targets, increments)

    large_size = 300_000
    large_targets = list(range(large_size))
    large_targets[0] = -1
    large_targets[-1] = large_size + 1
    write_case(
        args.out_dir,
        len(cases),
        large_targets,
        [1] * large_size,
    )


if __name__ == "__main__":
    main()
