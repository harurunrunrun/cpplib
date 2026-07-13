#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path

MOD = 998244353


def multiply(left: list[int], right: list[int], size: int) -> list[int]:
    result = [0] * size
    for i, x in enumerate(left):
        for j, y in enumerate(right):
            if i + j >= size:
                break
            result[i + j] = (result[i + j] + x * y) % MOD
    return result


def normalize(terms: list[tuple[int, int]], size: int) -> list[tuple[int, int]]:
    coefficients: dict[int, int] = {}
    for degree, coefficient in terms:
        if degree < size:
            coefficients[degree] = (coefficients.get(degree, 0) + coefficient) % MOD
    return sorted((degree, coefficient) for degree, coefficient in coefficients.items()
                  if coefficient)


def dense(terms: list[tuple[int, int]], size: int) -> list[int]:
    result = [0] * size
    for degree, coefficient in normalize(terms, size):
        result[degree] = coefficient
    return result


def inverse(values: list[int]) -> list[int]:
    size = len(values)
    result = [0] * size
    if size == 0:
        return result
    result[0] = pow(values[0], MOD - 2, MOD)
    for degree in range(1, size):
        total = sum(values[index] * result[degree - index]
                    for index in range(1, degree + 1)) % MOD
        result[degree] = -total * result[0] % MOD
    return result


def logarithm(values: list[int]) -> list[int]:
    size = len(values)
    if size == 0:
        return []
    derivative = [degree * values[degree] % MOD for degree in range(1, size)]
    quotient = multiply(derivative, inverse(values), size - 1)
    return [0] + [quotient[degree - 1] * pow(degree, MOD - 2, MOD) % MOD
                  for degree in range(1, size)]


def exponential(values: list[int]) -> list[int]:
    size = len(values)
    result = [0] * size
    if size == 0:
        return result
    result[0] = 1
    for degree in range(1, size):
        total = sum(index * values[index] * result[degree - index]
                    for index in range(1, degree + 1))
        result[degree] = total * pow(degree, MOD - 2, MOD) % MOD
    return result


def power(values: list[int], exponent: int) -> list[int]:
    size = len(values)
    result = [0] * size
    if size:
        result[0] = 1
    for _ in range(exponent):
        result = multiply(result, values, size)
    return result


def modular_sqrt(value: int) -> int | None:
    if value == 0:
        return 0
    if pow(value, (MOD - 1) // 2, MOD) != 1:
        return None
    odd, twos = MOD - 1, 0
    while odd % 2 == 0:
        odd //= 2
        twos += 1
    non_residue = 2
    while pow(non_residue, (MOD - 1) // 2, MOD) == 1:
        non_residue += 1
    root = pow(value, (odd + 1) // 2, MOD)
    remainder = pow(value, odd, MOD)
    base = pow(non_residue, odd, MOD)
    order = twos
    while remainder != 1:
        index, squared = 1, remainder * remainder % MOD
        while index < order and squared != 1:
            squared = squared * squared % MOD
            index += 1
        multiplier = pow(base, 1 << (order - index - 1), MOD)
        root = root * multiplier % MOD
        base = multiplier * multiplier % MOD
        remainder = remainder * base % MOD
        order = index
    return min(root, MOD - root)


def square_root(values: list[int]) -> list[int] | None:
    size = len(values)
    first = next((degree for degree, coefficient in enumerate(values) if coefficient), size)
    if first == size:
        return [0] * size
    if first % 2:
        return None
    leading_root = modular_sqrt(values[first])
    if leading_root is None:
        return None
    shift = first // 2
    result = [0] * size
    result[shift] = leading_root
    inverse_double_root = pow(2 * leading_root % MOD, MOD - 2, MOD)
    for offset in range(1, size - shift):
        degree = 2 * shift + offset
        target = values[degree] if degree < size else 0
        known = 0
        for left in range(shift + 1, shift + offset):
            right = degree - left
            known += result[left] * result[right]
        result[shift + offset] = (target - known) * inverse_double_root % MOD
    return result


def duplicated_terms(values: list[int], rng: random.Random) -> list[tuple[int, int]]:
    terms: list[tuple[int, int]] = []
    for degree, coefficient in enumerate(values):
        if coefficient == 0:
            continue
        split = rng.randrange(MOD)
        terms.extend([(degree, split), (degree, coefficient - split), (degree, 0)])
    terms.extend([(len(values) + 5, 91), (len(values) + 5, -91)])
    rng.shuffle(terms)
    return terms


def write_case(out_dir: Path, index: int, operation: str,
               terms: list[tuple[int, int]], size: int,
               answer: list[int] | None, exponent: int | None = None) -> None:
    suffix = f" {exponent}" if exponent is not None else ""
    lines = [f"{operation} {size} {len(terms)}{suffix}"]
    lines.extend(f"{degree} {coefficient}" for degree, coefficient in terms)
    (out_dir / f"case_{index:02d}.in").write_text(
        "\n".join(lines) + "\n", encoding="utf-8"
    )
    output = "-1" if answer is None else " ".join(map(str, answer))
    (out_dir / f"case_{index:02d}.out").write_text(output + "\n", encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(2026071317)
    cases: list[tuple[str, list[tuple[int, int]], int, list[int] | None, int | None]] = []

    cases.extend([
        ("INV", [(0, 1)], 0, [], None),
        ("POW", [], 6, [1, 0, 0, 0, 0, 0], 0),
        ("POW", [(2, 3), (2, -3), (4, 9)], 10, [0] * 10, 4),
        ("SQRT", [(3, 1)], 8, None, None),
        ("SQRT", [(0, 3)], 5, None, None),
        ("SQRT", [], 7, [0] * 7, None),
    ])

    for _ in range(30):
        size = rng.randrange(1, 34)
        values = [0] * size
        for _ in range(rng.randrange(1, min(size, 7) + 1)):
            values[rng.randrange(size)] = rng.randrange(1, MOD)
        operation = rng.choice(["INV", "LOG", "EXP", "POW", "SQRT"])
        exponent: int | None = None
        if operation == "INV":
            values[0] = rng.randrange(1, MOD)
            answer = inverse(values)
        elif operation == "LOG":
            values[0] = 1
            answer = logarithm(values)
        elif operation == "EXP":
            values[0] = 0
            answer = exponential(values)
        elif operation == "POW":
            exponent = rng.randrange(7)
            answer = power(values, exponent)
        else:
            sparse_root = [0] * size
            for _ in range(rng.randrange(1, min(size, 5) + 1)):
                sparse_root[rng.randrange(size)] = rng.randrange(1, 1000)
            values = multiply(sparse_root, sparse_root, size)
            answer = square_root(values)
        terms = duplicated_terms(values, rng)
        assert dense(terms, size) == values
        cases.append((operation, terms, size, answer, exponent))

    large_size = 1 << 15
    cases.extend([
        ("INV", [(0, 1), (1, -1)], large_size, [1] * large_size, None),
        ("SQRT", [(0, 1), (2, 2), (4, 1)], large_size,
         [1, 0, 1] + [0] * (large_size - 3), None),
    ])

    for index, (operation, terms, size, answer, exponent) in enumerate(cases):
        write_case(out_dir, index, operation, terms, size, answer, exponent)


if __name__ == "__main__":
    main()
