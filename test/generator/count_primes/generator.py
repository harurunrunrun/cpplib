#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from math import isqrt
from pathlib import Path


def is_prime_naive(value: int) -> bool:
    if value < 2:
        return False
    divisor = 2
    while divisor * divisor <= value:
        if value % divisor == 0:
            return False
        divisor += 1
    return True


def prime_table(maximum: int) -> list[bool]:
    prime = [True] * (maximum + 1)
    if maximum >= 0:
        prime[0] = False
    if maximum >= 1:
        prime[1] = False
    for value in range(2, isqrt(maximum) + 1):
        if not prime[value]:
            continue
        prime[value * value:maximum + 1:value] = (
            [False] * ((maximum - value * value) // value + 1)
        )
    return prime


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(20260714)
    cases: list[tuple[str, list[int], int | None]] = [
        ("M", [], None),
        ("S", [-7, -2, -1, 0, 1, 2, 3, 4, 5, 9, 97], 100),
        ("S", list(range(201)), 200),
        ("M", [99991, 99989, 99990, 100003, 100005], None),
        ("M", [rng.randrange(-100, 200001) for _ in range(400)], None),
    ]
    dense_values = list(range(500001))
    cases.append(("S", dense_values, 500000))
    input_lines = [str(len(cases))]
    output_lines = []
    dense_primes = prime_table(500000)
    for method, values, maximum in cases:
        header = f"{method} {len(values)}"
        if maximum is not None:
            header += f" {maximum}"
        input_lines.append(header)
        input_lines.append(" ".join(map(str, values)))
        if method == "S":
            output_lines.append(str(sum(
                dense_primes[value]
                for value in values
                if 0 <= value <= 500000
            )))
        else:
            output_lines.append(str(sum(
                is_prime_naive(value) for value in values
            )))

    (out_dir / "case_00.in").write_text(
        "\n".join(input_lines) + "\n", encoding="utf-8"
    )
    (out_dir / "case_00.out").write_text(
        "\n".join(output_lines) + "\n", encoding="utf-8"
    )


if __name__ == "__main__":
    main()
