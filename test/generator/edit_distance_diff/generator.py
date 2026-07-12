#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def encode(value: str) -> str:
    return value if value else "-"


def edit_distance(a: str, b: str, insert_cost: int, erase_cost: int, replace_cost: int) -> int:
    dp = [j * insert_cost for j in range(len(b) + 1)]
    for i, left in enumerate(a):
        next_dp = [(i + 1) * erase_cost] + [0] * len(b)
        for j, right in enumerate(b):
            next_dp[j + 1] = min(
                dp[j + 1] + erase_cost,
                next_dp[j] + insert_cost,
                dp[j] + (0 if left == right else replace_cost),
            )
        dp = next_dp
    return dp[-1]


def lcs_length(a: str, b: str) -> int:
    dp = [0] * (len(b) + 1)
    for left in a:
        next_dp = [0] * (len(b) + 1)
        for j, right in enumerate(b):
            if left == right:
                next_dp[j + 1] = dp[j] + 1
            else:
                next_dp[j + 1] = max(dp[j + 1], next_dp[j])
        dp = next_dp
    return dp[-1]


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    cases = [
        ("", "", 1, 1, 1),
        ("kitten", "sitting", 1, 1, 1),
        ("abc", "", 2, 3, 4),
        ("abcab", "bcba", 1, 1, 5),
    ]
    rng = random.Random(20260713)
    for _ in range(700):
        a = "".join(rng.choice("abcde") for _ in range(rng.randrange(31)))
        b = "".join(rng.choice("abcde") for _ in range(rng.randrange(31)))
        costs = (rng.randrange(1, 8), rng.randrange(1, 8), rng.randrange(1, 8))
        cases.append((a, b, *costs))

    lines = [str(len(cases))]
    outputs: list[str] = []
    for a, b, insert_cost, erase_cost, replace_cost in cases:
        lines.append(f"{encode(a)} {encode(b)} {insert_cost} {erase_cost} {replace_cost}")
        common = lcs_length(a, b)
        outputs.append(
            f"{edit_distance(a, b, insert_cost, erase_cost, replace_cost)} "
            f"{common} {len(b) - common} {len(a) - common} 1"
        )

    (out_dir / "case_00.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (out_dir / "case_00.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
