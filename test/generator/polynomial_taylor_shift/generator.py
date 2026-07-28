#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import math
import random
from pathlib import Path

MOD = 998244353


def shift_polynomial(coefficients: list[int], shift: int) -> list[int]:
    result = [0] * len(coefficients)
    for degree, coefficient in enumerate(coefficients):
        power = 1
        for target in range(degree, -1, -1):
            result[target] = (
                result[target]
                + coefficient * math.comb(degree, target) * power
            ) % MOD
            power = power * shift % MOD
    return result


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(0x71A710)
    cases: list[tuple[list[int], int]] = [
        ([], 0), ([7], 123), ([1, 2, 3], 0), ([1, 2, 3], 5)
    ]
    for size in [2, 4, 8, 17, 40, 100]:
        cases.append((
            [rng.randrange(MOD) for _ in range(size)],
            rng.randrange(-2 * MOD, 2 * MOD),
        ))
    input_lines = [str(len(cases))]
    output_lines = []
    for polynomial, shift in cases:
        input_lines.append(f"{len(polynomial)} {shift}")
        input_lines.append(" ".join(map(str, polynomial)))
        output_lines.append(" ".join(
            map(str, shift_polynomial(polynomial, shift % MOD))
        ))
    (args.out_dir / "case_00.in").write_text(
        "\n".join(input_lines) + "\n", encoding="utf-8"
    )
    (args.out_dir / "case_00.out").write_text(
        "\n".join(output_lines) + "\n", encoding="utf-8"
    )


if __name__ == "__main__":
    main()
