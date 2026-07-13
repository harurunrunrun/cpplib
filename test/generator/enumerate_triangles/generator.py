#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path

MOD = 998244353


def triangle_sum(n: int, weights: list[int], edges: list[tuple[int, int]]) -> int:
    degree = [0] * n
    for left, right in edges:
        degree[left] += 1
        degree[right] += 1
    outgoing = [[] for _ in range(n)]
    for left, right in edges:
        if (degree[left], left) > (degree[right], right):
            left, right = right, left
        outgoing[left].append(right)
    marked = [-1] * n
    answer = 0
    for first in range(n):
        for neighbor in outgoing[first]:
            marked[neighbor] = first
        for second in outgoing[first]:
            for third in outgoing[second]:
                if marked[third] == first:
                    answer += weights[first] * weights[second] % MOD * weights[third]
                    answer %= MOD
    return answer


def write_case(
    out_dir: Path,
    index: int,
    weights: list[int],
    edges: list[tuple[int, int]],
) -> None:
    lines = [f"{len(weights)} {len(edges)}", " ".join(map(str, weights))]
    lines.extend(f"{left} {right}" for left, right in edges)
    (out_dir / f"case_{index:02d}.in").write_text(
        "\n".join(lines) + "\n", encoding="utf-8"
    )
    (out_dir / f"case_{index:02d}.out").write_text(
        f"{triangle_sum(len(weights), weights, edges)}\n", encoding="utf-8"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    write_case(out_dir, 0, [1, 2, 3, 4, 5], [(0, 2), (0, 3), (2, 3), (1, 2), (1, 3)])
    for index, seed in enumerate([2026071491, 2026071492, 2026071493], start=1):
        rng = random.Random(seed)
        n = 90
        weights = [rng.randrange(MOD) for _ in range(n)]
        edges = [
            (left, right)
            for left in range(n)
            for right in range(left + 1, n)
            if rng.randrange(100) < 24
        ]
        write_case(out_dir, index, weights, edges)

    rng = random.Random(2026071494)
    n = 2400
    weights = [rng.randrange(MOD) for _ in range(n)]
    edges: list[tuple[int, int]] = []
    block = 24
    for start in range(0, n, block):
        end = min(n, start + block)
        edges.extend(
            (left, right)
            for left in range(start, end)
            for right in range(left + 1, end)
        )
    write_case(out_dir, 4, weights, edges)


if __name__ == "__main__":
    main()
