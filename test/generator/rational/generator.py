#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
from fractions import Fraction
import math
from pathlib import Path
import random


UNARY = ("normalize", "floor", "ceil", "trunc", "round", "round_even")
BINARY = ("add", "sub", "mul", "div", "cmp", "mediant")


def fraction_text(value: Fraction, *, unreduced: bool = False) -> str:
    if not unreduced or value.denominator == 1:
        return str(value.numerator) if value.denominator == 1 else (
            f"{value.numerator}/{value.denominator}"
        )
    factor = 6
    return f"{value.numerator * factor}/{value.denominator * factor}"


def random_fraction(rng: random.Random, digits: int) -> Fraction:
    lower = 10 ** (digits - 1) if digits > 1 else 1
    numerator = rng.randrange(lower, 10 ** digits)
    denominator = rng.randrange(lower, 10 ** digits)
    if rng.randrange(2):
        numerator = -numerator
    return Fraction(numerator, denominator)


def expected_unary(operation: str, value: Fraction) -> str:
    if operation == "normalize":
        result: Fraction | int = value
    elif operation == "floor":
        result = math.floor(value)
    elif operation == "ceil":
        result = math.ceil(value)
    elif operation == "trunc":
        result = math.trunc(value)
    elif operation == "round":
        result = math.floor(value + Fraction(1, 2)) if value >= 0 else (
            math.ceil(value - Fraction(1, 2))
        )
    else:
        result = round(value)
    return fraction_text(Fraction(result))


def expected_binary(operation: str, lhs: Fraction, rhs: Fraction) -> str:
    if operation == "add":
        result = lhs + rhs
    elif operation == "sub":
        result = lhs - rhs
    elif operation == "mul":
        result = lhs * rhs
    elif operation == "div":
        result = lhs / rhs
    elif operation == "cmp":
        return str((lhs > rhs) - (lhs < rhs))
    else:
        result = Fraction(
            lhs.numerator + rhs.numerator,
            lhs.denominator + rhs.denominator,
        )
    return fraction_text(result)


def make_case(seed: int, count: int, maximum_digits: int) -> tuple[str, str]:
    rng = random.Random(seed)
    queries: list[str] = []
    answers: list[str] = []
    edge_values = (
        Fraction(0), Fraction(1, 2), Fraction(-1, 2),
        Fraction(7, 3), Fraction(-7, 3),
    )
    for index in range(count):
        digits = 1 + rng.randrange(maximum_digits)
        lhs = edge_values[index] if index < len(edge_values) else random_fraction(rng, digits)
        if index % 7 == 0:
            operation = "pow"
            exponent = rng.randrange(-4, 5)
            if lhs == 0 and exponent < 0:
                exponent = 0
            queries.append(f"pow {fraction_text(lhs, unreduced=True)} {exponent}")
            answers.append(fraction_text(lhs ** exponent))
        elif index % 3 == 0:
            operation = UNARY[rng.randrange(len(UNARY))]
            queries.append(f"{operation} {fraction_text(lhs, unreduced=True)}")
            answers.append(expected_unary(operation, lhs))
        else:
            rhs = random_fraction(rng, digits)
            operation = BINARY[rng.randrange(len(BINARY))]
            queries.append(
                f"{operation} {fraction_text(lhs, unreduced=True)} "
                f"{fraction_text(rhs, unreduced=True)}"
            )
            answers.append(expected_binary(operation, lhs, rhs))
    return f"{len(queries)}\n" + "\n".join(queries) + "\n", "\n".join(answers) + "\n"


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    output = Path(args.out_dir)
    output.mkdir(parents=True, exist_ok=True)
    for index, parameters in enumerate(((0, 80, 3), (1, 120, 20), (2, 80, 160), (3, 24, 650))):
        input_text, output_text = make_case(*parameters)
        stem = f"case_{index:02d}"
        (output / f"{stem}.in").write_text(input_text, encoding="utf-8")
        (output / f"{stem}.out").write_text(output_text, encoding="utf-8")


if __name__ == "__main__":
    main()
