#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path

MOD = 998244353


def trim(poly: list[int]) -> list[int]:
    poly = [value % MOD for value in poly]
    while poly and poly[-1] == 0:
        poly.pop()
    return poly


def add(left: list[int], right: list[int], sign: int = 1) -> list[int]:
    result = [0] * max(len(left), len(right))
    for index, value in enumerate(left):
        result[index] = value
    for index, value in enumerate(right):
        result[index] = (result[index] + sign * value) % MOD
    return trim(result)


def multiply(left: list[int], right: list[int]) -> list[int]:
    result = [0] * max(0, len(left) + len(right) - 1)
    for i, x in enumerate(left):
        for j, y in enumerate(right):
            result[i + j] = (result[i + j] + x * y) % MOD
    return trim(result)


def divmod_poly(dividend: list[int], divisor: list[int]) -> tuple[list[int], list[int]]:
    dividend = trim(dividend)
    divisor = trim(divisor)
    quotient = [0] * max(0, len(dividend) - len(divisor) + 1)
    inverse = pow(divisor[-1], MOD - 2, MOD)
    while len(dividend) >= len(divisor):
        shift = len(dividend) - len(divisor)
        coefficient = dividend[-1] * inverse % MOD
        quotient[shift] = coefficient
        for index, value in enumerate(divisor):
            dividend[shift + index] = (
                dividend[shift + index] - coefficient * value
            ) % MOD
        dividend = trim(dividend)
    return trim(quotient), dividend


def inverse_mod(value: list[int], modulus: list[int]) -> list[int]:
    old_remainder, remainder = modulus, divmod_poly(value, modulus)[1]
    old_coefficient: list[int] = []
    coefficient = [1]
    while remainder:
        quotient, next_remainder = divmod_poly(old_remainder, remainder)
        old_remainder, remainder = remainder, next_remainder
        old_coefficient, coefficient = coefficient, add(
            old_coefficient, multiply(quotient, coefficient), -1
        )
    scale = pow(old_remainder[0], MOD - 2, MOD)
    return divmod_poly([(value * scale) % MOD for value in old_coefficient], modulus)[1]


def decompose(
    numerator: list[int], factors: list[list[int]]
) -> tuple[list[int], list[list[int]]]:
    denominator = [1]
    for factor in factors:
        denominator = multiply(denominator, factor)
    whole, remainder = divmod_poly(numerator, denominator)
    parts = []
    for factor in factors:
        complement, zero = divmod_poly(denominator, factor)
        assert not zero
        local = divmod_poly(remainder, factor)[1]
        parts.append(divmod_poly(multiply(local, inverse_mod(complement, factor)), factor)[1])
    return whole, parts


def encoded(poly: list[int]) -> str:
    return " ".join([str(len(poly)), *map(str, poly)])


def write_case(
    out_dir: Path,
    index: int,
    numerator: list[int],
    factors: list[list[int]],
) -> None:
    input_lines = [f"{len(numerator)} {len(factors)}", " ".join(map(str, numerator))]
    input_lines.extend(encoded(factor) for factor in factors)
    whole, parts = decompose(numerator, factors)
    output_lines = [encoded(whole), str(len(parts))]
    output_lines.extend(encoded(part) for part in parts)
    (out_dir / f"case_{index:03d}.in").write_text(
        "\n".join(input_lines) + "\n", encoding="utf-8"
    )
    (out_dir / f"case_{index:03d}.out").write_text(
        "\n".join(output_lines) + "\n", encoding="utf-8"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)

    write_case(args.out_dir, 0, [1, 2, 3], [])
    write_case(args.out_dir, 1, [7, -2, 4, 9], [[-1, 1], [-3, 1]])
    write_case(args.out_dir, 2, [5, 0, 6, 1, 8, 4], [[4, -4, 1], [-7, 1]])

    rng = random.Random(2026071614)
    for case_index in range(3, 43):
        factor_count = rng.randint(1, 8)
        roots = rng.sample(range(1, 1000000), factor_count)
        factors = [[-root, 1] for root in roots]
        numerator = [rng.randrange(MOD) for _ in range(rng.randint(0, factor_count + 8))]
        write_case(args.out_dir, case_index, numerator, factors)


if __name__ == "__main__":
    main()
