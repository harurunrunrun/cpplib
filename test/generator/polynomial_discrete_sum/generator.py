#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import math
import random
from pathlib import Path


MOD = 998244353


def evaluate(coefficients: list[int], x: int) -> int:
    result = 0
    for coefficient in reversed(coefficients):
        result = (result * x + coefficient) % MOD
    return result


def discrete_antiderivative(coefficients: list[int]) -> list[int]:
    count = len(coefficients)
    result = [0] * (count + 1)
    for degree in reversed(range(count)):
        residual = coefficients[degree] % MOD
        for target_degree in range(degree + 2, count + 1):
            residual -= result[target_degree] * math.comb(target_degree, degree)
        result[degree + 1] = residual * pow(degree + 1, MOD - 2, MOD) % MOD
    return result


def prefix_naive(coefficients: list[int], n: int) -> int:
    return sum(evaluate(coefficients, index) for index in range(n)) % MOD


def power_naive(n: int, exponent: int) -> int:
    return sum(pow(index, exponent, MOD) for index in range(n)) % MOD


def faulhaber_value(n: int, exponent: int) -> int:
    coefficients = [0] * exponent + [1]
    return evaluate(discrete_antiderivative(coefficients), n)


def write_case(
    out_dir: Path, index: int, queries: list[str], answers: list[str]
) -> None:
    assert len(queries) == len(answers)
    stem = f"case_{index:02d}"
    (out_dir / f"{stem}.in").write_text(
        "\n".join([str(len(queries)), *queries]) + "\n", encoding="utf-8"
    )
    (out_dir / f"{stem}.out").write_text(
        "\n".join(answers) + "\n", encoding="utf-8"
    )


def fixed_case(out_dir: Path) -> None:
    queries = [
        "P 0 0",
        "P 100 0",
        "P 10 1",
        "P 10 2",
        "F -7 5",
        "D 0",
        "D 3 4 -2 7",
        "S 0 1000000000000000000",
        "S 3 30 4 -2 7",
        "M 3 -19 4 -2 7",
    ]
    polynomial = [4, -2, 7]
    answers = [
        "0",
        "100",
        "45",
        "285",
        str(faulhaber_value(-7, 5)),
        "0",
        " ".join(map(str, discrete_antiderivative(polynomial))),
        "0",
        str(prefix_naive(polynomial, 30)),
        str(evaluate(discrete_antiderivative(polynomial), -19)),
    ]
    write_case(out_dir, 0, queries, answers)


def random_case(out_dir: Path) -> None:
    rng = random.Random(2026071322)
    queries: list[str] = []
    answers: list[str] = []
    for iteration in range(300):
        operation = iteration % 5
        if operation == 0:
            n = rng.randrange(101)
            exponent = rng.randrange(21)
            queries.append(f"P {n} {exponent}")
            answers.append(str(power_naive(n, exponent)))
        elif operation == 1:
            n = rng.randrange(-10**18, 10**18)
            exponent = rng.randrange(21)
            queries.append(f"F {n} {exponent}")
            answers.append(str(faulhaber_value(n, exponent)))
        else:
            count = rng.randrange(22)
            polynomial = [rng.randrange(-10**9, 10**9) for _ in range(count)]
            coefficients = " ".join(map(str, polynomial))
            if operation == 2:
                queries.append(f"D {count}" + (f" {coefficients}" if count else ""))
                answers.append(" ".join(map(str, discrete_antiderivative(polynomial))))
            elif operation == 3:
                n = rng.randrange(101)
                queries.append(f"S {count} {n}" + (f" {coefficients}" if count else ""))
                answers.append(str(prefix_naive(polynomial, n)))
            else:
                n = rng.randrange(-10**18, 10**18)
                queries.append(f"M {count} {n}" + (f" {coefficients}" if count else ""))
                answers.append(str(evaluate(discrete_antiderivative(polynomial), n)))
    write_case(out_dir, 1, queries, answers)


def large_case(out_dir: Path) -> None:
    exponent = 100000
    prefix_degree = 1500
    coefficient_count = prefix_degree + 1
    telescoping = [math.comb(prefix_degree + 1, degree) % MOD
                   for degree in range(coefficient_count)]
    n = 10**18
    coefficient_text = " ".join(map(str, telescoping))
    queries = [
        f"P {MOD} {exponent}",
        f"P {MOD + 5} {exponent}",
        f"P {10**18} 0",
        f"D {coefficient_count} {coefficient_text}",
        f"S {coefficient_count} {n} {coefficient_text}",
    ]
    expected_antiderivative = [0] * (prefix_degree + 1) + [1]
    answers = [
        "0",
        str(power_naive(5, exponent)),
        str(10**18 % MOD),
        " ".join(map(str, expected_antiderivative)),
        str(pow(n, prefix_degree + 1, MOD)),
    ]
    write_case(out_dir, 2, queries, answers)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)
    fixed_case(args.out_dir)
    random_case(args.out_dir)
    large_case(args.out_dir)


if __name__ == "__main__":
    main()
