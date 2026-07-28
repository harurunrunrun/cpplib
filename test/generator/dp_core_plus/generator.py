#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import itertools
import random
from pathlib import Path


def knapsack(capacity: int, weights: list[int]) -> tuple[int, str]:
    reachable = {0}
    for weight in weights:
        reachable |= {
            value + weight
            for value in list(reachable)
            if value + weight <= capacity
        }
    return max(reachable), "".join(
        str(int(value in reachable)) for value in range(capacity + 1)
    )


def lcs_length(first: str, second: str) -> int:
    dp = [0] * (len(second) + 1)
    for left in first:
        previous = dp[:]
        for index, right in enumerate(second):
            dp[index + 1] = (
                previous[index] + 1
                if left == right
                else max(dp[index], previous[index + 1])
            )
    return dp[-1]


def exact_cover_count(column_count: int, rows: list[list[int]]) -> int:
    rows = [row for row in rows if row]
    answer = 0
    for mask in range(1 << len(rows)):
        covered = [0] * column_count
        for row_id, row in enumerate(rows):
            if mask >> row_id & 1:
                for column in row:
                    covered[column] += 1
        answer += all(value == 1 for value in covered)
    return answer


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    parser.add_argument("--seed", type=int, default=20260728)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(args.seed)
    cases: list[tuple[int, object, str]] = []
    for _ in range(160):
        capacity = rng.randint(0, 180)
        weights = [rng.randint(0, 70) for _ in range(rng.randint(0, 35))]
        maximum, bits = knapsack(capacity, weights)
        cases.append((0, (capacity, weights), f"{maximum}{bits}"))
    alphabet = "abcd"
    for _ in range(160):
        first = "".join(rng.choice(alphabet) for _ in range(rng.randint(1, 45)))
        second = "".join(rng.choice(alphabet) for _ in range(rng.randint(1, 45)))
        cases.append((1, (first, second), str(lcs_length(first, second))))
    for _ in range(180):
        column_count = rng.randint(0, 8)
        row_count = rng.randint(0, 12)
        rows = []
        for _ in range(row_count):
            row = [
                column
                for column in range(column_count)
                if rng.random() < 0.3
            ]
            rows.append(row)
        cases.append((
            2,
            (column_count, rows),
            str(exact_cover_count(column_count, rows)),
        ))
    input_lines = [str(len(cases))]
    output_lines = []
    for kind, data, expected in cases:
        if kind == 0:
            capacity, weights = data
            input_lines.append(f"0 {capacity} {len(weights)}")
            input_lines.append(" ".join(map(str, weights)))
        elif kind == 1:
            first, second = data
            input_lines.append(f"1 {first} {second}")
        else:
            column_count, rows = data
            input_lines.append(f"2 {column_count} {len(rows)}")
            input_lines.extend(
                " ".join(map(str, [len(row), *row])) for row in rows
            )
        output_lines.append(expected)
    (args.out_dir / "cases.in").write_text(
        "\n".join(input_lines) + "\n", encoding="utf-8"
    )
    (args.out_dir / "cases.out").write_text(
        "\n".join(output_lines) + "\n", encoding="utf-8"
    )


if __name__ == "__main__":
    main()
