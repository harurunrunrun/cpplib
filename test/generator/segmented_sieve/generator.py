#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import math
import random
from pathlib import Path


def is_prime(value: int) -> bool:
    if value < 2:
        return False
    for divisor in range(2, math.isqrt(value) + 1):
        if value % divisor == 0:
            return False
    return True


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(20260713)
    ranges = [
        (0, 0),
        (0, 3),
        (1, 2),
        (2, 3),
        (10, 20),
        (100, 130),
        (999_999_990_000, 999_999_990_050),
        (1_000_000_000_000, 1_000_000_000_080),
    ]
    for _ in range(120):
        left = rng.randrange(2_000_000)
        ranges.append((left, left + rng.randrange(201)))

    input_lines = [str(len(ranges))]
    output_lines: list[str] = []
    for left, right in ranges:
        input_lines.append(f"{left} {right}")
        primes = [value for value in range(left, right) if is_prime(value)]
        output_lines.append(" ".join(map(str, [len(primes), *primes])))

    (out_dir / "case_00.in").write_text("\n".join(input_lines) + "\n", encoding="utf-8")
    (out_dir / "case_00.out").write_text("\n".join(output_lines) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
