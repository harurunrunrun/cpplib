#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def exact_knapsack(weights: list[int], values: list[int], capacity: int) -> int:
    dp = [0] * (capacity + 1)
    for weight, value in zip(weights, values):
        if weight > capacity:
            continue
        for current in range(capacity, weight - 1, -1):
            dp[current] = max(dp[current], dp[current - weight] + value)
    return max(dp)


def exact_subset(values: list[int], capacity: int) -> int:
    reachable = 1
    mask = (1 << (capacity + 1)) - 1
    for value in values:
        if value <= capacity:
            reachable |= reachable << value
            reachable &= mask
    return reachable.bit_length() - 1


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(2026071319)

    knapsack: list[tuple[list[int], list[int], int, str]] = [
        ([], [], 0, "0.25"),
        ([0, 0, 4, 7], [5, 0, 9, 20], 6, "0.20"),
        ([5, 6], [10, 12], 10, "0.50"),
        ([101, 102], [1000, 1001], 100, "0.125"),
        ([1, 1, 1, 1], [0, 1, 2, 3], 2, "0.333"),
    ]
    epsilon_values = ["0.05", "0.10", "0.20", "0.37", "0.75"]
    for _ in range(70):
        size = rng.randrange(0, 19)
        capacity = rng.randrange(0, 181)
        weights = [rng.randrange(0, 101) for _ in range(size)]
        values = [rng.randrange(0, 3001) for _ in range(size)]
        knapsack.append((weights, values, capacity, rng.choice(epsilon_values)))

    subsets: list[tuple[list[int], int, str]] = [
        ([], 0, "0.25"),
        ([0, 0, 0], 0, "0.50"),
        ([1, 2, 4, 8, 16], 23, "0.10"),
        ([100, 101, 102], 99, "0.75"),
        ([7, 7, 7, 7], 15, "0.333"),
    ]
    for _ in range(70):
        size = rng.randrange(0, 25)
        capacity = rng.randrange(0, 401)
        values = [rng.randrange(0, 251) for _ in range(size)]
        subsets.append((values, capacity, rng.choice(epsilon_values)))

    input_lines = [str(len(knapsack))]
    output_lines: list[str] = []
    for weights, values, capacity, epsilon in knapsack:
        input_lines.append(f"{len(weights)} {capacity} {epsilon}")
        input_lines.append(" ".join(map(str, weights)))
        input_lines.append(" ".join(map(str, values)))
        output_lines.append(f"K {exact_knapsack(weights, values, capacity)}")
    input_lines.append(str(len(subsets)))
    for values, capacity, epsilon in subsets:
        input_lines.append(f"{len(values)} {capacity} {epsilon}")
        input_lines.append(" ".join(map(str, values)))
        output_lines.append(f"S {exact_subset(values, capacity)}")

    (out_dir / "case_00.in").write_text("\n".join(input_lines) + "\n", encoding="utf-8")
    (out_dir / "case_00.out").write_text("\n".join(output_lines) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
