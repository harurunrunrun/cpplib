#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def is_probable_prime(value: int) -> bool:
    if value < 2:
        return False
    for prime in (2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37):
        if value % prime == 0:
            return value == prime
    odd = value - 1
    power = 0
    while odd % 2 == 0:
        odd //= 2
        power += 1
    for base in (2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41):
        current = pow(base, odd, value)
        if current in (1, value - 1):
            continue
        for _ in range(power - 1):
            current = current * current % value
            if current == value - 1:
                break
        else:
            return False
    return True


def next_prime(value: int) -> int:
    value |= 1
    while not is_probable_prime(value):
        value += 2
    return value


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    directory = Path(args.out_dir)
    directory.mkdir(parents=True, exist_ok=True)

    rng = random.Random(2026071622)
    cases: list[tuple[int, list[tuple[int, int]], int, int, int, int]] = []
    explicit = [
        [(101, 1), (103, 1)],
        [(1009, 2), (10007, 1)],
        [(1000003, 1), (1000033, 1)],
        [(2147483647, 1), (2305843009213693951, 1)],
        [(2, 63), (3, 12), (101, 1)],
    ]
    for factors in explicit:
        value = 1
        for prime, exponent in factors:
            value *= prime ** exponent
        cases.append((value, factors, 2000, 30000, 32, rng.getrandbits(64)))
    large_factors = [
        (9223372036854775783, 1),
        (18446744073709551557, 1),
    ]
    cases.append((
        170141183460469230726339751698713544131,
        large_factors,
        20000,
        400000,
        48,
        0x6A09E667F3BCC909,
    ))

    for _ in range(24):
        first = next_prime(rng.randrange(10_000, 2_000_000))
        second = next_prime(rng.randrange(2_000_001, 50_000_000))
        exponent = 2 if rng.randrange(5) == 0 else 1
        factors = sorted([(first, exponent), (second, 1)])
        if factors[0][0] == factors[1][0]:
            factors = [(first, exponent + 1)]
        value = first ** exponent * second
        if rng.randrange(3) == 0:
            value = -value
        cases.append((value, factors, 1000, 15000, 20, rng.getrandbits(64)))

    commands: list[str] = []
    output: list[str] = []
    for value, factors, bound1, bound2, curves, seed in cases:
        commands.append(f"{value} {bound1} {bound2} {curves} {seed}")
        output.append(str(len(factors)) + " " + " ".join(
            f"{prime} {exponent}" for prime, exponent in factors
        ))

    (directory / "case_00.in").write_text(
        str(len(commands)) + "\n" + "\n".join(commands) + "\n",
        encoding="utf-8",
    )
    (directory / "case_00.out").write_text(
        "\n".join(output) + "\n", encoding="utf-8"
    )


if __name__ == "__main__":
    main()
