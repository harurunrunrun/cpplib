#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import itertools
import random
from pathlib import Path


def count(values: list[int], modulus: int) -> int:
    total = 1 % modulus
    previous: dict[int, int] = {}
    for value in values:
        duplicated = previous.get(value, 0)
        previous[value] = total
        total = (2 * total - duplicated) % modulus
    return (total - 1) % modulus


def brute(values: list[int], modulus: int) -> int:
    found = set()
    for mask in range(1, 1 << len(values)):
        found.add(tuple(values[i] for i in range(len(values)) if mask >> i & 1))
    return len(found) % modulus


def write_case(out_dir: Path, index: int,
               cases: list[tuple[list[int], int]], brute_only: bool = False) -> None:
    parts = [str(len(cases))]
    answers = []
    for values, modulus in cases:
        parts.append(f"{len(values)} {modulus}")
        parts.append(" ".join(map(str, values)))
        answers.append(str(brute(values, modulus) if brute_only
                           else count(values, modulus)))
    (out_dir / f"case_{index:02d}.in").write_text(
        "\n".join(parts) + "\n", encoding="utf-8"
    )
    (out_dir / f"case_{index:02d}.out").write_text(
        "\n".join(answers) + "\n", encoding="utf-8"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)

    small = []
    for length in range(9):
        small.extend((list(values), modulus)
                     for values in itertools.product(range(3), repeat=length)
                     for modulus in [1, 2, 7, 998244353])
    for offset in range(0, len(small), 700):
        write_case(args.out_dir, offset // 700, small[offset:offset + 700], True)

    rng = random.Random(2026071311)
    random_cases = []
    for _ in range(300):
        size = rng.randrange(1, 500)
        alphabet = rng.randrange(1, 100)
        random_cases.append((
            [rng.randrange(alphabet) for _ in range(size)],
            rng.randrange(1, 10**9 + 1),
        ))
    write_case(args.out_dir, 40, random_cases)
    write_case(args.out_dir, 41, [
        ([7] * 500000, 998244353),
        (list(range(500000)), 998244353),
    ])


if __name__ == "__main__":
    main()
