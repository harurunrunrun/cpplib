#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import math
import random
from pathlib import Path


def merge(first: tuple[int, int], second: tuple[int, int]) -> tuple[int, int] | None:
    first_remainder, first_modulus = first
    second_remainder, second_modulus = second
    gcd = math.gcd(first_modulus, second_modulus)
    difference = second_remainder - first_remainder
    if difference % gcd:
        return None
    reduced = second_modulus // gcd
    multiplier = difference // gcd * pow(first_modulus // gcd, -1, reduced) % reduced
    modulus = first_modulus // gcd * second_modulus
    return ((first_remainder + first_modulus * multiplier) % modulus, modulus)


def primitive_root(prime: int) -> int:
    value = prime - 1
    factors: list[int] = []
    divisor = 2
    while divisor * divisor <= value:
        if value % divisor == 0:
            factors.append(divisor)
            while value % divisor == 0:
                value //= divisor
        divisor += 1
    if value > 1:
        factors.append(value)
    for candidate in range(1, prime):
        if all(pow(candidate, (prime - 1) // factor, prime) != 1 for factor in factors):
            return candidate
    raise AssertionError


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(20260713)
    commands: list[str] = []
    outputs: list[str] = []
    for _ in range(350):
        count = rng.randrange(1, 7)
        equations = [
            (rng.randrange(-1000, 1001), rng.randrange(1, 300))
            for _ in range(count)
        ]
        result: tuple[int, int] | None = (0, 1)
        for equation in equations:
            if result is not None:
                result = merge(result, equation)
        commands.append(
            "CRT " + str(count) + " " + " ".join(f"{r} {m}" for r, m in equations)
        )
        outputs.append("NONE" if result is None else f"{result[0]} {result[1]}")

    for prime in (2, 3, 5, 17, 97, 65537, 998244353):
        commands.append(f"ROOT {prime}")
        outputs.append(str(primitive_root(prime)))

    (out_dir / "case_00.in").write_text(
        f"{len(commands)}\n" + "\n".join(commands) + "\n", encoding="utf-8"
    )
    (out_dir / "case_00.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
