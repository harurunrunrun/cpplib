#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path

MAXIMUM = (1 << 63) - 1


def solve(weights: list[int], group_count: int) -> int:
    size = len(weights)
    infinity = 10**100
    previous = [infinity] * (size + 1)
    previous[0] = 0
    for group in range(1, group_count + 1):
        current = [infinity] * (size + 1)
        for end in range(group, size + 1):
            suffix_weight = 0
            cost = 0
            for start in range(end - 1, group - 2, -1):
                current[end] = min(
                    current[end],
                    previous[start] + cost,
                )
                suffix_weight += weights[start]
                cost += suffix_weight
        previous = current
    return previous[size]


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[tuple[list[int], int]] = [
        ([1], 1),
        ([1, 2, 3], 1),
        ([1, 2, 3], 3),
        ([5, 1, 7, 2], 2),
        ([MAXIMUM, MAXIMUM], 1),
        ([MAXIMUM, MAXIMUM, MAXIMUM], 1),
    ]
    source = random.Random(0x1E47E)
    for _ in range(180):
        size = source.randint(1, 22)
        weights = [source.randint(1, 1000) for _ in range(size)]
        cases.append((weights, source.randint(1, size)))

    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for weights, group_count in cases:
        input_lines.append(f"{len(weights)} {group_count}")
        input_lines.append(" ".join(map(str, weights)))
        answer = solve(weights, group_count)
        output_lines.append(str(answer) if answer <= MAXIMUM else "overflow")

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
