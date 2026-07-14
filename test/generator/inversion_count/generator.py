#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def answer(values: list[int]) -> int:
    ordered = sorted(set(values))
    rank = {value: index for index, value in enumerate(ordered)}
    bit = [0] * (len(ordered) + 1)

    def prefix_sum(end: int) -> int:
        result = 0
        while end:
            result += bit[end]
            end -= end & -end
        return result

    result = 0
    for index, value in enumerate(values):
        position = rank[value]
        result += index - prefix_sum(position + 1)
        position += 1
        while position < len(bit):
            bit[position] += 1
            position += position & -position
    return result


def write_case(out_dir: Path, index: int, values: list[int]) -> None:
    (out_dir / f"case_{index:02d}.in").write_text(
        f"{len(values)}\n" + " ".join(map(str, values)) + "\n",
        encoding="utf-8",
    )
    (out_dir / f"case_{index:02d}.out").write_text(
        f"{answer(values)}\n",
        encoding="utf-8",
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)

    cases = [
        [],
        [1],
        [1, 2, 3, 4, 5],
        [5, 4, 3, 2, 1],
        [3, 1, 2, 1],
        [-(10**18), 10**18, 0, -(10**18), 10**18],
    ]
    rng = random.Random(2026071404)
    for _ in range(30):
        size = rng.randrange(0, 120)
        cases.append([rng.randrange(-10**9, 10**9 + 1) for _ in range(size)])

    for index, values in enumerate(cases):
        write_case(args.out_dir, index, values)

    large_size = 300_000
    write_case(args.out_dir, len(cases), list(range(large_size, 0, -1)))


if __name__ == "__main__":
    main()
