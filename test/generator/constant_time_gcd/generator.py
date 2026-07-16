#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import math
import random
from pathlib import Path


def write_case(directory: Path, index: int, maximum: int,
               pairs: list[tuple[int, int]]) -> None:
    (directory / f"case_{index:02d}.in").write_text(
        f"{maximum} {len(pairs)}\n"
        + "".join(f"{first} {second}\n" for first, second in pairs),
        encoding="utf-8",
    )
    (directory / f"case_{index:02d}.out").write_text(
        "".join(f"{math.gcd(first, second)}\n" for first, second in pairs),
        encoding="utf-8",
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    directory = Path(args.out_dir)
    directory.mkdir(parents=True, exist_ok=True)

    write_case(directory, 0, 0, [(0, 0)] * 20)
    write_case(directory, 1, 200, [
        (first, second) for first in range(201) for second in range(201)
    ])
    rng = random.Random(2026071656)
    maximum = 1_000_000
    pairs = [(0, maximum), (maximum, 0), (999_983, 999_983)]
    pairs += [
        (rng.randrange(maximum + 1), rng.randrange(maximum + 1))
        for _ in range(100_000)
    ]
    write_case(directory, 2, maximum, pairs)


if __name__ == "__main__":
    main()
