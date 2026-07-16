#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


MOD = 998244353


def naive_polya_exp(values: list[int]) -> list[int]:
    size = len(values)
    exponent = [0] * size
    for degree in range(1, size):
        for multiplier in range(1, (size - 1) // degree + 1):
            exponent[degree * multiplier] = (
                exponent[degree * multiplier]
                + values[degree] * pow(multiplier, MOD - 2, MOD)
            ) % MOD
    result = [0] * size
    if size:
        result[0] = 1
    for degree in range(1, size):
        total = sum(
            index * exponent[index] * result[degree - index]
            for index in range(1, degree + 1)
        )
        result[degree] = total * pow(degree, MOD - 2, MOD) % MOD
    return result


def line(values: list[int]) -> str:
    return " ".join(map(str, values))


def partitions(size: int) -> list[int]:
    result = [0] * size
    if size:
        result[0] = 1
    for n in range(1, size):
        value = 0
        k = 1
        while k * (3 * k - 1) // 2 <= n:
            first = k * (3 * k - 1) // 2
            second = k * (3 * k + 1) // 2
            sign = 1 if k & 1 else -1
            value += sign * result[n - first]
            if second <= n:
                value += sign * result[n - second]
            k += 1
        result[n] = value % MOD
    return result


def write_suite(out_dir: Path, name: str, cases: list[list[int]], expected: list[list[int]]) -> None:
    (out_dir / f"{name}.in").write_text(
        str(len(cases)) + "\n" + "\n".join(
            f"{len(values)}\n{line(values)}" for values in cases
        ) + "\n",
        encoding="utf-8",
    )
    output: list[str] = []
    for values, exponential in zip(cases, expected):
        output.append(line(exponential))
        output.append(line(values))
    (out_dir / f"{name}.out").write_text("\n".join(output) + "\n", encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(20260716)
    cases = [[0], [0, 1], [0, 1, 0, 0, 0, 0], [0, 0, 1, 0, 0, 0]]
    for _ in range(120):
        size = rng.randrange(1, 81)
        cases.append([0] + [rng.randrange(MOD) for _ in range(size - 1)])
    write_suite(out_dir, "case_00", cases, [naive_polya_exp(v) for v in cases])

    size = 4096
    atom = [0] + [1] * (size - 1)
    write_suite(out_dir, "case_01", [atom], [partitions(size)])


if __name__ == "__main__":
    main()
