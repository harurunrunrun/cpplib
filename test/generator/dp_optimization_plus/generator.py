#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def optimized_layer(previous: list[int], values: list[int]) -> int:
    prefix = [0]
    for value in values:
        prefix.append(prefix[-1] + value)
    end = len(values)
    return min(
        previous[split] + (prefix[end] - prefix[split]) ** 2
        for split in range(end)
    ) if end else 10**30


def interval_merge(weights: list[int]) -> int:
    size = len(weights)
    prefix = [0]
    for value in weights:
        prefix.append(prefix[-1] + value)
    dp = [[0] * (size + 1) for _ in range(size + 1)]
    for length in range(2, size + 1):
        for left in range(size - length + 1):
            right = left + length
            dp[left][right] = (
                min(
                    dp[left][split] + dp[split][right]
                    for split in range(left + 1, right)
                )
                + prefix[right] - prefix[left]
            )
    return dp[0][size]


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    parser.add_argument("--seed", type=int, default=20260728)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(args.seed)
    cases: list[tuple[int, object, int]] = []
    for _ in range(180):
        size = rng.randint(1, 100)
        previous = [rng.randint(-1000, 1000) for _ in range(size + 1)]
        values = [rng.randint(0, 20) for _ in range(size)]
        cases.append((0, (previous, values), optimized_layer(previous, values)))
    for _ in range(140):
        weights = [rng.randint(0, 30) for _ in range(rng.randint(0, 45))]
        cases.append((1, weights, interval_merge(weights)))
    for _ in range(160):
        count = rng.randint(1, 80)
        values = rng.sample(range(-900, 901), count)
        target = rng.randint(0, count)
        cases.append((2, (target, values), sum(sorted(values)[:target])))
    input_lines = [str(len(cases))]
    output_lines = []
    for kind, data, expected in cases:
        if kind == 0:
            previous, values = data
            input_lines.append(f"0 {len(values)}")
            input_lines.append(" ".join(map(str, previous)))
            input_lines.append(" ".join(map(str, values)))
        elif kind == 1:
            input_lines.append(f"1 {len(data)}")
            input_lines.append(" ".join(map(str, data)))
        else:
            target, values = data
            input_lines.append(f"2 {len(values)} {target}")
            input_lines.append(" ".join(map(str, values)))
        output_lines.append(str(expected))
    (args.out_dir / "cases.in").write_text(
        "\n".join(input_lines) + "\n", encoding="utf-8"
    )
    (args.out_dir / "cases.out").write_text(
        "\n".join(output_lines) + "\n", encoding="utf-8"
    )


if __name__ == "__main__":
    main()
