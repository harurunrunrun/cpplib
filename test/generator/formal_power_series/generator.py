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


def inverse(values: list[int], size: int) -> list[int]:
    result = [pow(values[0], MOD - 2, MOD)]
    for degree in range(1, size):
        total = sum(values[i] * result[degree - i]
                    for i in range(1, min(degree + 1, len(values)))) % MOD
        result.append(-total * result[0] % MOD)
    return result


def derivative(values: list[int]) -> list[int]:
    return [i * values[i] % MOD for i in range(1, len(values))]


def integral(values: list[int]) -> list[int]:
    return [0] + [value * pow(i + 1, MOD - 2, MOD) % MOD
                  for i, value in enumerate(values)]


def logarithm(values: list[int], size: int) -> list[int]:
    return integral(multiply(derivative(values), inverse(values, size), size - 1))[:size]


def exponential(values: list[int], size: int) -> list[int]:
    result = [0] * size
    if size == 0:
        return result
    result[0] = 1
    for degree in range(1, size):
        total = 0
        for index in range(1, min(degree + 1, len(values))):
            total += index * values[index] * result[degree - index]
        result[degree] = total * pow(degree, MOD - 2, MOD) % MOD
    return result


def power(values: list[int], exponent: int, size: int) -> list[int]:
    result = [0] * size
    if size:
        result[0] = 1
    base = values[:size] + [0] * max(0, size - len(values))
    for _ in range(exponent):
        result = multiply(result, base, size)
    return result


def modular_sqrt(value: int) -> int | None:
    if value == 0:
        return 0
    if pow(value, (MOD - 1) // 2, MOD) != 1:
        return None
    if MOD % 4 == 3:
        root = pow(value, (MOD + 1) // 4, MOD)
        return min(root, MOD - root)
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
    first = next((i for i, value in enumerate(values) if value), size)
    if first == size:
        return [0] * size
    if first % 2:
        return None
    root_constant = modular_sqrt(values[first])
    if root_constant is None:
        return None
    shift = first // 2
    result = [0] * size
    result[shift] = root_constant
    inverse_double_root = pow(2 * root_constant % MOD, MOD - 2, MOD)
    for offset in range(1, size - shift):
        degree = 2 * shift + offset
        target = values[degree] if degree < size else 0
        known = 0
        for left in range(shift + 1, shift + offset):
            right = degree - left
            if 0 <= right < size:
                known += result[left] * result[right]
        result[shift + offset] = (target - known) * inverse_double_root % MOD
    return result


def write_case(out_dir: Path, index: int, operation: str,
               values: list[int], answer: list[int] | None,
               exponent: int | None = None) -> None:
    suffix = f" {exponent}" if exponent is not None else ""
    (out_dir / f"case_{index:02d}.in").write_text(
        f"{operation} {len(values)}{suffix}\n" + " ".join(map(str, values)) + "\n",
        encoding="utf-8",
    )
    output = "-1" if answer is None else " ".join(map(str, answer))
    (out_dir / f"case_{index:02d}.out").write_text(output + "\n", encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(2026071305)
    cases: list[tuple[str, list[int], list[int] | None, int | None]] = []
    cases.extend([
        ("DERIVATIVE", [], [], None),
        ("INTEGRAL", [], [0], None),
        ("SQRT", [0, 1], None, None),
        ("SQRT", [3], None, None),
        ("POW", [0, 0, 5, 7], [1, 0, 0, 0], 0),
    ])
    for _ in range(24):
        size = rng.randrange(1, 38)
        values = [rng.randrange(MOD) for _ in range(size)]
        operation = rng.choice(["INV", "LOG", "EXP", "POW", "SQRT", "DERIVATIVE", "INTEGRAL"])
        exponent: int | None = None
        if operation == "INV":
            values[0] = rng.randrange(1, MOD)
            answer = inverse(values, size)
        elif operation == "LOG":
            values[0] = 1
            answer = logarithm(values, size)
        elif operation == "EXP":
            values[0] = 0
            answer = exponential(values, size)
        elif operation == "POW":
            exponent = rng.randrange(7)
            answer = power(values, exponent, size)
        elif operation == "SQRT":
            root = [rng.randrange(1000) for _ in range(size)]
            if rng.randrange(3) == 0 and size > 2:
                root[0] = root[1] = 0
            values = multiply(root, root, size)
            answer = square_root(values)
        elif operation == "DERIVATIVE":
            answer = derivative(values)
        else:
            answer = integral(values)
        cases.append((operation, values, answer, exponent))

    large_size = 1 << 15
    large_values = [1, MOD - 1] + [0] * (large_size - 2)
    cases.append(("INV", large_values, [1] * large_size, None))
    for index, (operation, values, answer, exponent) in enumerate(cases):
        write_case(out_dir, index, operation, values, answer, exponent)


if __name__ == "__main__":
    main()
