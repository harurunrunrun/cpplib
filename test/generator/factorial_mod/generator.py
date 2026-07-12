#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def factorial_without_prime(n: int, prime: int) -> tuple[int, int]:
    value = 1
    exponent = 0
    for x in range(1, n + 1):
        remainder = x
        while remainder % prime == 0:
            remainder //= prime
            exponent += 1
        value = value * (remainder % prime) % prime
    return value, exponent


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(20260713)
    primes = [2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 101, 997]
    cases = [(3, 0), (3, 10), (5, 24)]
    for _ in range(280):
        cases.append((rng.choice(primes), rng.randrange(5001)))

    inputs = [str(len(cases))]
    outputs: list[str] = []
    for prime, n in cases:
        inputs.append(f"{prime} {n}")
        value, exponent = factorial_without_prime(n, prime)
        outputs.append(f"{value} {exponent}")

    (out_dir / "case_00.in").write_text("\n".join(inputs) + "\n", encoding="utf-8")
    (out_dir / "case_00.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
