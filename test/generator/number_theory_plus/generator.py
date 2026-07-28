#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import fractions
import math
import random
from pathlib import Path


def continued_fraction(numerator: int, denominator: int) -> list[int]:
    result = []
    while denominator:
        quotient = numerator // denominator
        result.append(quotient)
        numerator, denominator = denominator, numerator - quotient * denominator
    if len(result) > 1 and result[-1] == 1:
        result.pop()
        result[-1] += 1
    return result


def farey(order: int) -> list[fractions.Fraction]:
    return sorted(
        {
            fractions.Fraction(numerator, denominator)
            for denominator in range(1, order + 1)
            for numerator in range(denominator + 1)
        }
    )


def pell(nonsquare: int) -> tuple[int, int] | None:
    root = math.isqrt(nonsquare)
    if root * root == nonsquare:
        return None
    offset, denominator, coefficient = 0, 1, root
    p0, p1, q0, q1 = 0, 1, 1, 0
    while True:
        x = coefficient * p1 + p0
        y = coefficient * q1 + q0
        if x * x - nonsquare * y * y == 1:
            return x, y
        p0, p1, q0, q1 = p1, x, q1, y
        offset = denominator * coefficient - offset
        denominator = (nonsquare - offset * offset) // denominator
        coefficient = (root + offset) // denominator


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    parser.add_argument("--seed", type=int, default=20260728)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(args.seed)
    cases: list[tuple[int, tuple[object, ...], str]] = []
    primes = [2, 3, 5, 7, 11, 13, 17, 19, 29, 43, 97, 193]
    for _ in range(180):
        prime = rng.choice(primes)
        exponent = rng.randint(1, 30)
        value = rng.randrange(prime)
        roots = [
            root
            for root in range(prime)
            if pow(root, exponent, prime) == value
        ]
        cases.append((
            0,
            (value, exponent, prime),
            " ".join(map(str, [len(roots), *roots])),
        ))
    pairwise_moduli = [[3, 5, 7], [8, 9, 5], [11, 13], [17]]
    for _ in range(100):
        moduli = rng.choice(pairwise_moduli)
        value = rng.randrange(math.prod(moduli))
        output_modulus = rng.randint(1, 10**9)
        remainders = [value % modulus for modulus in moduli]
        cases.append((
            1,
            (len(moduli), output_modulus, remainders, moduli),
            str(value % output_modulus),
        ))
    for _ in range(120):
        denominator = rng.choice([
            value for value in range(-100, 101) if value
        ])
        numerator = rng.randint(-1000, 1000)
        coefficients = continued_fraction(numerator, denominator)
        cases.append((
            2,
            (numerator, denominator),
            " ".join(map(str, [len(coefficients), *coefficients])),
        ))
    for order in range(1, 24):
        sequence = farey(order)
        output = [str(len(sequence))]
        output.extend(
            f"{value.numerator}/{value.denominator}" for value in sequence
        )
        cases.append((3, (order,), " ".join(output)))
    reconstruction_primes = [1009, 2003, 5003, 10007]
    for _ in range(140):
        modulus = rng.choice(reconstruction_primes)
        denominator_bound = rng.randint(1, 20)
        numerator_bound = rng.randint(0, 20)
        while True:
            denominator = rng.randint(1, denominator_bound)
            numerator = rng.randint(-numerator_bound, numerator_bound)
            if math.gcd(numerator, denominator) == 1:
                break
        residue = numerator * pow(denominator, -1, modulus) % modulus
        cases.append((
            4,
            (residue, modulus, numerator_bound, denominator_bound),
            f"{numerator} {denominator}",
        ))
    for nonsquare in range(1, 130):
        solution = pell(nonsquare)
        expected = (
            "none" if solution is None else f"{solution[0]} {solution[1]}"
        )
        cases.append((5, (nonsquare,), expected))
    input_lines = [str(len(cases))]
    output_lines = []
    for kind, data, expected in cases:
        if kind == 1:
            count, output_modulus, remainders, moduli = data
            input_lines.append(f"{kind} {count} {output_modulus}")
            input_lines.append(" ".join(map(str, remainders)))
            input_lines.append(" ".join(map(str, moduli)))
        else:
            input_lines.append(" ".join(map(str, (kind, *data))))
        output_lines.append(expected)
    (args.out_dir / "cases.in").write_text(
        "\n".join(input_lines) + "\n", encoding="utf-8"
    )
    (args.out_dir / "cases.out").write_text(
        "\n".join(output_lines) + "\n", encoding="utf-8"
    )


if __name__ == "__main__":
    main()
