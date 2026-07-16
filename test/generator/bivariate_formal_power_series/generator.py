#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path

MOD = 998244353
SIZE = 6
MAX_TOTAL = 10


def zero() -> list[list[int]]:
    return [[0] * SIZE for _ in range(SIZE)]


def constant(value: int) -> list[list[int]]:
    result = zero()
    result[0][0] = value % MOD
    return result


def add(left: list[list[int]], right: list[list[int]], sign: int = 1) -> list[list[int]]:
    return [[(left[x][y] + sign * right[x][y]) % MOD
             for y in range(SIZE)] for x in range(SIZE)]


def multiply(left: list[list[int]], right: list[list[int]]) -> list[list[int]]:
    result = zero()
    for ax in range(SIZE):
        for ay in range(SIZE):
            for bx in range(SIZE - ax):
                for by in range(SIZE - ay):
                    result[ax + bx][ay + by] = (
                        result[ax + bx][ay + by] + left[ax][ay] * right[bx][by]
                    ) % MOD
    return result


def inverse(value: list[list[int]]) -> list[list[int]]:
    result = zero()
    inv_constant = pow(value[0][0], MOD - 2, MOD)
    result[0][0] = inv_constant
    for total in range(1, MAX_TOTAL + 1):
        for x in range(max(0, total - 5), min(5, total) + 1):
            y = total - x
            current = 0
            for ax in range(x + 1):
                for ay in range(y + 1):
                    if ax == 0 and ay == 0:
                        continue
                    current += value[ax][ay] * result[x - ax][y - ay]
            result[x][y] = -current * inv_constant % MOD
    return result


def logarithm(value: list[list[int]]) -> list[list[int]]:
    term = [row[:] for row in value]
    term[0][0] = (term[0][0] - 1) % MOD
    power_value = [row[:] for row in term]
    result = zero()
    for exponent in range(1, MAX_TOTAL + 1):
        scale = pow(exponent, MOD - 2, MOD)
        contribution = [[item * scale % MOD for item in row] for row in power_value]
        result = add(result, contribution, 1 if exponent % 2 else -1)
        if exponent != MAX_TOTAL:
            power_value = multiply(power_value, term)
    return result


def exponential(value: list[list[int]]) -> list[list[int]]:
    result = constant(1)
    power_value = constant(1)
    factorial = 1
    for exponent in range(1, MAX_TOTAL + 1):
        power_value = multiply(power_value, value)
        factorial = factorial * exponent % MOD
        scale = pow(factorial, MOD - 2, MOD)
        contribution = [[item * scale % MOD for item in row] for row in power_value]
        result = add(result, contribution)
    return result


def series_pow(value: list[list[int]], exponent: int) -> list[list[int]]:
    if exponent < 0:
        value = inverse(value)
        exponent = -exponent
    result = constant(1)
    base = [row[:] for row in value]
    while exponent:
        if exponent & 1:
            result = multiply(result, base)
        exponent >>= 1
        if exponent:
            base = multiply(base, base)
    return result


def flatten(value: list[list[int]]) -> str:
    return " ".join(str(value[x][y]) for x in range(SIZE) for y in range(SIZE))


def random_series(rng: random.Random) -> list[list[int]]:
    return [[rng.randrange(MOD) for _ in range(SIZE)] for _ in range(SIZE)]


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    directory = Path(args.out_dir)
    directory.mkdir(parents=True, exist_ok=True)

    rng = random.Random(2026071620)
    commands: list[str] = []
    output: list[str] = []
    for _ in range(80):
        operation = rng.randrange(5)
        value = random_series(rng)
        if operation == 0:
            value[0][0] = rng.randrange(1, MOD)
            commands.append("INV " + flatten(value))
            result = inverse(value)
        elif operation == 1:
            value[0][0] = 1
            commands.append("LOG " + flatten(value))
            result = logarithm(value)
        elif operation == 2:
            value[0][0] = 0
            commands.append("EXP " + flatten(value))
            result = exponential(value)
        elif operation == 3:
            value[0][0] = rng.randrange(1, MOD)
            exponent = rng.randrange(-20, 21)
            commands.append(f"POW {exponent} " + flatten(value))
            result = series_pow(value, exponent)
        else:
            root = value
            if root[0][0] == 0:
                root[0][0] = 1
            value = multiply(root, root)
            commands.append(f"SQRT {root[0][0]} " + flatten(value))
            result = root
        output.append(flatten(result))

    (directory / "case_00.in").write_text(
        str(len(commands)) + "\n" + "\n".join(commands) + "\n", encoding="utf-8"
    )
    (directory / "case_00.out").write_text(
        "\n".join(output) + "\n", encoding="utf-8"
    )


if __name__ == "__main__":
    main()

