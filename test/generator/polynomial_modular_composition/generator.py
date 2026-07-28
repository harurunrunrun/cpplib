#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path

MOD = 998244353


def trim(values: list[int]) -> list[int]:
    while values and values[-1] % MOD == 0:
        values.pop()
    return values


def remainder(values: list[int], modulus: list[int]) -> list[int]:
    values = trim(values[:])
    inverse = pow(modulus[-1], MOD - 2, MOD)
    while len(values) >= len(modulus):
        factor = values[-1] * inverse % MOD
        shift = len(values) - len(modulus)
        for index, coefficient in enumerate(modulus):
            values[shift + index] = (
                values[shift + index] - factor * coefficient
            ) % MOD
        trim(values)
    return values


def multiply_mod(left: list[int], right: list[int], modulus: list[int]) -> list[int]:
    if not left or not right:
        return []
    product = [0] * (len(left) + len(right) - 1)
    for i, first in enumerate(left):
        for j, second in enumerate(right):
            product[i + j] = (product[i + j] + first * second) % MOD
    return remainder(product, modulus)


def compose_mod(outer: list[int], inner: list[int], modulus: list[int]) -> list[int]:
    result: list[int] = []
    for coefficient in reversed(outer):
        result = multiply_mod(result, inner, modulus)
        if result:
            result[0] = (result[0] + coefficient) % MOD
        elif coefficient:
            result = [coefficient]
        trim(result)
    return result


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    parser.add_argument("--seed", type=int, default=20260728)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(args.seed)
    cases = [
        ([1, 2, 3], [0, 1], [1, 0, 1]),
        ([4, 0, 5, 2], [3, 1], [2, 3, 0, 1]),
        ([], [1, 2], [1, 1]),
    ]
    for _ in range(100):
        degree = rng.randint(1, 22)
        modulus = [rng.randrange(MOD) for _ in range(degree)]
        modulus.append(rng.randrange(1, MOD))
        outer = [rng.randrange(MOD) for _ in range(rng.randint(0, 35))]
        inner = [rng.randrange(MOD) for _ in range(rng.randint(0, 28))]
        cases.append((outer, inner, modulus))

    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for outer, inner, modulus in cases:
        input_lines.append(f"{len(outer)} {len(inner)} {len(modulus)}")
        input_lines.append(" ".join(map(str, outer)))
        input_lines.append(" ".join(map(str, inner)))
        input_lines.append(" ".join(map(str, modulus)))
        answer = compose_mod(outer, inner, modulus)
        output_lines.append(
            str(len(answer)) + (" " + " ".join(map(str, answer)) if answer else "")
        )
    (args.out_dir / "cases.in").write_text(
        "\n".join(input_lines) + "\n", encoding="utf-8"
    )
    (args.out_dir / "cases.out").write_text(
        "\n".join(output_lines) + "\n", encoding="utf-8"
    )


if __name__ == "__main__":
    main()
