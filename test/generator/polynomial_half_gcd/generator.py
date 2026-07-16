#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


MOD = 998244353


def trim(values: list[int]) -> list[int]:
    while values and values[-1] % MOD == 0:
        values.pop()
    return [value % MOD for value in values]


def multiply(left: list[int], right: list[int]) -> list[int]:
    if not left or not right:
        return []
    result = [0] * (len(left) + len(right) - 1)
    for i, first in enumerate(left):
        for j, second in enumerate(right):
            result[i + j] = (result[i + j] + first * second) % MOD
    return trim(result)


def remainder(first: list[int], second: list[int]) -> list[int]:
    work = trim(first[:])
    divisor = trim(second[:])
    inverse = pow(divisor[-1], MOD - 2, MOD)
    while len(work) >= len(divisor):
        scale = work[-1] * inverse % MOD
        shift = len(work) - len(divisor)
        for index, coefficient in enumerate(divisor):
            work[index + shift] = (work[index + shift] - scale * coefficient) % MOD
        trim(work)
    return work


def gcd(first: list[int], second: list[int]) -> list[int]:
    first, second = trim(first[:]), trim(second[:])
    while second:
        first, second = second, remainder(first, second)
    if not first:
        return []
    inverse = pow(first[-1], MOD - 2, MOD)
    return [value * inverse % MOD for value in first]


def serialize(polynomial: list[int]) -> str:
    return " ".join(map(str, polynomial))


def write(out_dir: Path, name: str, cases: list[tuple[list[int], list[int]]], answers: list[list[int]]) -> None:
    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for (first, second), answer in zip(cases, answers):
        input_lines.append(f"{len(first)} {len(second)}")
        input_lines.append(serialize(first))
        input_lines.append(serialize(second))
        output_lines.append(str(len(answer)) + (" " + serialize(answer) if answer else ""))
    (out_dir / f"{name}.in").write_text("\n".join(input_lines) + "\n", encoding="utf-8")
    (out_dir / f"{name}.out").write_text("\n".join(output_lines) + "\n", encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(20260716)
    cases: list[tuple[list[int], list[int]]] = [
        ([], []),
        ([0, 6], []),
        ([2, 3, 1], [1, 1]),
        ([1, 0, 1], [1, 1]),
        ([0], [0]),
    ]
    for _ in range(180):
        common = [rng.randrange(MOD) for _ in range(rng.randrange(1, 8))]
        common[-1] = rng.randrange(1, MOD)
        left = [rng.randrange(MOD) for _ in range(rng.randrange(1, 18))]
        right = [rng.randrange(MOD) for _ in range(rng.randrange(1, 18))]
        left[-1] = rng.randrange(1, MOD)
        right[-1] = rng.randrange(1, MOD)
        cases.append((multiply(common, left), multiply(common, right)))
    for _ in range(20):
        common = [rng.randrange(MOD) for _ in range(rng.randrange(4, 16))]
        common[-1] = rng.randrange(1, MOD)
        left = [rng.randrange(MOD) for _ in range(rng.randrange(40, 81))]
        right = [rng.randrange(MOD) for _ in range(rng.randrange(40, 81))]
        left[-1] = rng.randrange(1, MOD)
        right[-1] = rng.randrange(1, MOD)
        cases.append((multiply(common, left), multiply(common, right)))
    write(out_dir, "case_00", cases, [gcd(a, b) for a, b in cases])

    first = [0] * 4097
    first[0] = first[-1] = 1
    second = [0] * 2049
    second[-1] = 1
    write(out_dir, "case_01", [(first, second)], [[1]])


if __name__ == "__main__":
    main()
