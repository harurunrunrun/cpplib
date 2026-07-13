#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


MOD = 998244353


def evaluate(coefficients: list[int], x: int) -> int:
    result = 0
    for coefficient in reversed(coefficients):
        result = (result * x + coefficient) % MOD
    return result


def equidistant_query(
    coefficients: list[int], first: int, step: int, x: int, sample_count: int
) -> tuple[str, str]:
    values = [evaluate(coefficients, first + step * index)
              for index in range(sample_count)]
    query = " ".join(map(str, ["E", sample_count, first, step, x, *values]))
    return query, str(evaluate(coefficients, x))


def coefficient_query(coefficients: list[int], xs: list[int]) -> tuple[str, str]:
    ys = [evaluate(coefficients, x) for x in xs]
    query = " ".join(map(str, ["C", len(xs), *xs, *ys]))
    answer = " ".join(str(value % MOD) for value in coefficients)
    return query, answer


def arbitrary_query(coefficients: list[int], xs: list[int], x: int) -> tuple[str, str]:
    ys = [evaluate(coefficients, point) for point in xs]
    query = " ".join(map(str, ["A", len(xs), x, *xs, *ys]))
    return query, str(evaluate(coefficients, x))


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
    queries: list[str] = []
    answers: list[str] = []

    for query, answer in [
        equidistant_query([37], -100, 9, 10**18, 1),
        equidistant_query([5, -7, 3, 11], -13, 6, -13 + 6 * 3, 8),
        equidistant_query([2, 0, 9], 80, -5, -10**12, 7),
        coefficient_query([91], [-17]),
        coefficient_query([8, -3, 0, 5], [-9, -1, 4, 12]),
        arbitrary_query([4, 1, -6, 2, 9], [-15, -4, 0, 7, 19], 123456789),
    ]:
        queries.append(query)
        answers.append(answer)
    write_case(out_dir, 0, queries, answers)


def random_case(out_dir: Path) -> None:
    rng = random.Random(2026071321)
    queries: list[str] = []
    answers: list[str] = []
    for iteration in range(360):
        sample_count = rng.randrange(1, 25)
        coefficients = [rng.randrange(MOD) for _ in range(sample_count)]
        if iteration % 3 == 0:
            first = rng.randrange(-10**9, 10**9)
            step = rng.choice([value for value in range(-20, 21) if value != 0])
            if iteration % 9 == 0:
                x = first + step * rng.randrange(sample_count)
            else:
                x = rng.randrange(-10**18, 10**18)
            query, answer = equidistant_query(
                coefficients, first, step, x, sample_count
            )
        else:
            xs = rng.sample(range(-300, 301), sample_count)
            if iteration % 3 == 1:
                query, answer = coefficient_query(coefficients, xs)
            else:
                x = xs[rng.randrange(sample_count)] if iteration % 10 == 2 else rng.randrange(-10**18, 10**18)
                query, answer = arbitrary_query(coefficients, xs, x)
        queries.append(query)
        answers.append(answer)
    write_case(out_dir, 1, queries, answers)


def large_case(out_dir: Path) -> None:
    sample_count = 200001
    degree = sample_count - 1
    x = 10**18
    values = [0] * degree + [1]
    query = " ".join(map(str, ["E", sample_count, 0, 1, x, *values]))

    numerator = 1
    denominator = 1
    reduced_x = x % MOD
    for index in range(degree):
        numerator = numerator * (reduced_x - index) % MOD
        denominator = denominator * (index + 1) % MOD
    answer = numerator * pow(denominator, MOD - 2, MOD) % MOD
    write_case(out_dir, 2, [query], [str(answer)])


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
