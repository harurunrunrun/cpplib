#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import collections
import random
from pathlib import Path


def majority(values: list[int]) -> int | None:
    value, count = collections.Counter(values).most_common(1)[0]
    return value if len(values) // 2 < count else None


def write_case(out_dir: Path, index: int, values: list[int], rank: int) -> None:
    selected = sorted(values)[rank]
    candidate = majority(values)
    (out_dir / f"case_{index:02d}.in").write_text(
        f"{len(values)} {rank}\n" + " ".join(map(str, values)) + "\n",
        encoding="utf-8",
    )
    (out_dir / f"case_{index:02d}.out").write_text(
        f"{selected}\n{candidate if candidate is not None else 'NONE'}\n",
        encoding="utf-8",
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[tuple[list[int], int]] = [
        ([0], 0),
        ([7] * 101, 73),
        ([1, 2] * 50, 0),
        ([3, 1, 3, 2, 3, 3, 0], 4),
        (list(range(2000)), 1000),
        (list(range(1999, -1, -1)), 666),
    ]
    rng = random.Random(2026071309)
    for _ in range(35):
        size = rng.randrange(1, 500)
        values = [rng.randrange(-30, 31) for _ in range(size)]
        if rng.randrange(3) == 0:
            winner = rng.randrange(-10, 11)
            for index in rng.sample(range(size), size // 2 + 1):
                values[index] = winner
        cases.append((values, rng.randrange(size)))

    for index, (values, rank) in enumerate(cases):
        write_case(args.out_dir, index, values, rank)


if __name__ == "__main__":
    main()
