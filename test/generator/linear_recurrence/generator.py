#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path

MOD = 998244353


def combine(lhs: list[int], rhs: list[int], coefficient: list[int]) -> list[int]:
    degree = len(coefficient)
    product = [0] * (2 * degree - 1)
    for i, x in enumerate(lhs):
        for j, y in enumerate(rhs):
            product[i + j] = (product[i + j] + x * y) % MOD
    for exponent in range(2 * degree - 2, degree - 1, -1):
        value = product[exponent]
        for j, c in enumerate(coefficient):
            product[exponent - 1 - j] = (
                product[exponent - 1 - j] + value * c
            ) % MOD
    return product[:degree]


def kth(initial: list[int], coefficient: list[int], index: int) -> int:
    degree = len(coefficient)
    if degree == 0:
        return 0
    if index < degree:
        return initial[index]
    result = [1] + [0] * (degree - 1)
    base = ([0, 1] + [0] * (degree - 2)) if degree > 1 else [coefficient[0]]
    while index:
        if index & 1:
            result = combine(result, base, coefficient)
        base = combine(base, base, coefficient)
        index >>= 1
    return sum(x * y for x, y in zip(result, initial)) % MOD


def extend(initial: list[int], coefficient: list[int], length: int) -> list[int]:
    sequence = initial[:]
    degree = len(coefficient)
    while len(sequence) < length:
        sequence.append(
            sum(coefficient[j] * sequence[-1 - j] for j in range(degree)) % MOD
        )
    return sequence


def write_case(
    out_dir: Path,
    case_id: int,
    case_type: str,
    sequence: list[int],
    coefficient: list[int],
    index: int,
    answer: int,
) -> None:
    lines = [f"{case_type} {len(sequence)} {index}", " ".join(map(str, sequence))]
    if case_type == "KNOWN":
        lines.append(" ".join(map(str, coefficient)))
    (out_dir / f"case_{case_id:02d}.in").write_text(
        "\n".join(lines) + "\n", encoding="utf-8"
    )
    (out_dir / f"case_{case_id:02d}.out").write_text(
        f"{answer}\n", encoding="utf-8"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(20260713)

    known_cases = [
        ([0], [0], 0),
        ([7], [1], 10**18),
        ([0, 1], [1, 1], 10**18 + 123),
    ]
    for degree in (2, 3, 8, 31, 64, 128):
        initial = [rng.randrange(MOD) for _ in range(degree)]
        coefficient = [rng.randrange(MOD) for _ in range(degree)]
        known_cases.append((initial, coefficient, rng.randrange(10**18)))

    case_id = 0
    for initial, coefficient, index in known_cases:
        write_case(
            out_dir, case_id, "KNOWN", initial, coefficient, index,
            kth(initial, coefficient, index),
        )
        case_id += 1

    write_case(out_dir, case_id, "INFER", [], [], 10**18, 0)
    case_id += 1
    write_case(out_dir, case_id, "INFER", [0] * 40, [], 10**18, 0)
    case_id += 1
    for degree in (1, 2, 5, 16, 48):
        initial = [rng.randrange(MOD) for _ in range(degree)]
        coefficient = [rng.randrange(MOD) for _ in range(degree)]
        coefficient[-1] = rng.randrange(1, MOD)
        sequence = extend(initial, coefficient, 2 * degree + 24)
        index = rng.randrange(10**18)
        write_case(
            out_dir, case_id, "INFER", sequence, coefficient, index,
            kth(initial, coefficient, index),
        )
        case_id += 1


if __name__ == "__main__":
    main()
