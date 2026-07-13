#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def optimal_cost(cost: list[list[int]]) -> int:
    rows = len(cost)
    columns = len(cost[0]) if rows else 0
    infinity = 10**30
    dp = [infinity] * (1 << columns)
    dp[0] = 0
    for row in range(rows):
        nxt = [infinity] * (1 << columns)
        for mask, value in enumerate(dp):
            if value == infinity or mask.bit_count() != row:
                continue
            for column in range(columns):
                if not (mask >> column) & 1:
                    next_mask = mask | (1 << column)
                    nxt[next_mask] = min(nxt[next_mask], value + cost[row][column])
        dp = nxt
    return min(value for mask, value in enumerate(dp) if mask.bit_count() == rows)


def write_case(out_dir: Path, index: int, cost: list[list[int]], expected: int | None = None) -> None:
    rows = len(cost)
    columns = len(cost[0]) if rows else 0
    if expected is None:
        expected = optimal_cost(cost)
    lines = [f"{rows} {columns}"]
    lines.extend(" ".join(map(str, row)) for row in cost)
    (out_dir / f"case_{index:02d}.in").write_text(
        "\n".join(lines) + "\n", encoding="utf-8"
    )
    (out_dir / f"case_{index:02d}.out").write_text(
        f"{expected}\n", encoding="utf-8"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    write_case(out_dir, 0, [[5, -4, 7], [2, 3, -8]])
    for index, seed in enumerate([2026071441, 2026071442, 2026071443], start=1):
        rng = random.Random(seed)
        rows = 7 + index % 2
        columns = 9
        cost = [[rng.randrange(-10**9, 10**9 + 1) for _ in range(columns)] for _ in range(rows)]
        write_case(out_dir, index, cost)

    n = 160
    cost = [
        [(row - column) ** 2 - 10**9 + row for column in range(n)]
        for row in range(n)
    ]
    write_case(out_dir, 4, cost, -10**9 * n + n * (n - 1) // 2)


if __name__ == "__main__":
    main()
