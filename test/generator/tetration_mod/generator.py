#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import math
import random
from pathlib import Path


def totient(value: int) -> int:
    result = value
    remaining = value
    prime = 2
    while prime * prime <= remaining:
        if remaining % prime == 0:
            result = result // prime * (prime - 1)
            while remaining % prime == 0:
                remaining //= prime
        prime += 1
    if remaining > 1:
        result = result // remaining * (remaining - 1)
    return result


def at_least(base: int, height: int, limit: int) -> bool:
    if limit == 0:
        return True
    if height == 0:
        return 1 >= limit
    if base == 0:
        return (1 if height % 2 == 0 else 0) >= limit
    if base == 1:
        return 1 >= limit
    required = 0
    power = 1
    while power < limit:
        required += 1
        power *= base
    return at_least(base, height - 1, required)


def solve(base: int, height: int, modulus: int) -> int:
    if modulus == 1:
        return 0
    if height == 0:
        return 1 % modulus
    phi = totient(modulus)
    exponent = solve(base, height - 1, phi)
    if at_least(base, height - 1, phi):
        exponent += phi
    return pow(base, exponent, modulus)


def write_case(out_dir: Path, index: int,
               cases: list[tuple[int, int, int]]) -> None:
    input_data = str(len(cases)) + "\n" + "".join(
        f"{a} {b} {m}\n" for a, b, m in cases
    )
    output_data = "".join(f"{solve(a, b, m)}\n" for a, b, m in cases)
    (out_dir / f"case_{index:02d}.in").write_text(input_data, encoding="utf-8")
    (out_dir / f"case_{index:02d}.out").write_text(output_data, encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)

    fixed = [(a, b, m) for a in range(7) for b in range(7)
             for m in range(1, 35)]
    fixed += [
        (0, 10**9, 10**9), (1, 10**9, 998244353),
        (2, 10**9, 2**30), (10**9, 10**9, 10**9),
        (998244353, 998244353, 998244353),
    ]
    for offset in range(0, len(fixed), 400):
        write_case(args.out_dir, offset // 400, fixed[offset:offset + 400])

    rng = random.Random(2026071309)
    random_cases = [
        (rng.randrange(10**9 + 1), rng.randrange(10**9 + 1),
         rng.randrange(1, 10**9 + 1))
        for _ in range(1000)
    ]
    write_case(args.out_dir, 5, random_cases)


if __name__ == "__main__":
    main()
