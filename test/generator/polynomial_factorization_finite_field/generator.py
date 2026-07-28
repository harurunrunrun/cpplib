#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path

MOD = 998244353


def multiply(left: list[int], right: list[int]) -> list[int]:
    result = [0] * (len(left) + len(right) - 1)
    for i, first in enumerate(left):
        for j, second in enumerate(right):
            result[i + j] = (result[i + j] + first * second) % MOD
    return result


def power(polynomial: list[int], exponent: int) -> list[int]:
    result = [1]
    while exponent:
        if exponent & 1:
            result = multiply(result, polynomial)
        exponent >>= 1
        if exponent:
            polynomial = multiply(polynomial, polynomial)
    return result


def key(polynomial: list[int]) -> tuple[int, tuple[int, ...]]:
    return len(polynomial), tuple(reversed(polynomial))


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    parser.add_argument("--seed", type=int, default=20260728)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(args.seed)
    cases: list[tuple[int, list[tuple[list[int], int]]]] = [
        (7, []),
        (3, [([MOD - 1, 1], 1)]),
        (5, [([MOD - 3, 0, 1], 1)]),
        (11, [([MOD - 2, 1], 3), ([MOD - 3, 0, 1], 2)]),
    ]
    for _ in range(70):
        factors: list[tuple[list[int], int]] = []
        used: set[tuple[int, ...]] = set()
        for _ in range(rng.randint(1, 6)):
            if rng.randrange(2) == 0:
                root = rng.randrange(MOD)
                factor = [(-root) % MOD, 1]
            else:
                shift = rng.randrange(MOD)
                factor = [
                    (shift * shift - 3) % MOD,
                    (-2 * shift) % MOD,
                    1,
                ]
            signature = tuple(factor)
            if signature in used:
                continue
            used.add(signature)
            factors.append((factor, rng.randint(1, 4)))
        cases.append((rng.randrange(1, MOD), factors))

    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for leading, factors in cases:
        polynomial = [leading]
        for factor, multiplicity in factors:
            polynomial = multiply(polynomial, power(factor, multiplicity))
        input_lines.append(str(len(polynomial)))
        input_lines.append(" ".join(map(str, polynomial)))
        ordered = sorted(factors, key=lambda item: key(item[0]))
        output_lines.append(f"{leading} {len(ordered)}")
        for factor, multiplicity in ordered:
            output_lines.append(
                f"{multiplicity} {len(factor)} "
                + " ".join(map(str, factor))
            )
    (args.out_dir / "cases.in").write_text(
        "\n".join(input_lines) + "\n", encoding="utf-8"
    )
    (args.out_dir / "cases.out").write_text(
        "\n".join(output_lines) + "\n", encoding="utf-8"
    )


if __name__ == "__main__":
    main()
