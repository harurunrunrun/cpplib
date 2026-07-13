#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import math
import random
from pathlib import Path


def naive_table(limit: int, modulus: int, q: int) -> list[list[int]]:
    table = [[0] * (limit + 1) for _ in range(limit + 1)]
    table[0][0] = 1
    powers = [1]
    for _ in range(limit):
        powers.append(powers[-1] * q % modulus)
    for n in range(1, limit + 1):
        for k in range(n + 1):
            table[n][k] = table[n - 1][k]
            if k != 0:
                table[n][k] += powers[n - k] * table[n - 1][k - 1]
                table[n][k] %= modulus
    return table


def write_case(out_dir: Path, index: int, modulus: int, q: int,
               queries: list[tuple[int, int]], answers: list[int]) -> None:
    (out_dir / f"case_{index:02d}.in").write_text(
        f"{len(queries)} {modulus} {q}\n" +
        "".join(f"{n} {k}\n" for n, k in queries),
        encoding="utf-8",
    )
    (out_dir / f"case_{index:02d}.out").write_text(
        "".join(f"{value}\n" for value in answers),
        encoding="utf-8",
    )


def small_case(out_dir: Path, index: int, modulus: int, q: int) -> None:
    limit = min(modulus - 1, 70)
    table = naive_table(limit, modulus, q)
    queries = [(n, k) for n in range(limit + 1)
               for k in range(limit + 1)]
    answers = [table[n][k] if k <= n else 0 for n, k in queries]
    write_case(out_dir, index, modulus, q, queries, answers)


def q_lucas_minus_one(n: int, k: int, modulus: int) -> int:
    if k > n or k % 2 > n % 2:
        return 0
    return math.comb(n // 2, k // 2) % modulus


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)

    small_case(args.out_dir, 0, 2, 0)
    small_case(args.out_dir, 1, 3, 2)
    small_case(args.out_dir, 2, 5, 4)
    small_case(args.out_dir, 3, 7, 1)
    small_case(args.out_dir, 4, 7, 2)
    small_case(args.out_dir, 5, 101, 0)

    rng = random.Random(2026071303)
    primes = [11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47]
    for offset, modulus in enumerate(primes):
        q = rng.randrange(modulus)
        table = naive_table(modulus - 1, modulus, q)
        queries = [(rng.randrange(modulus), rng.randrange(modulus))
                   for _ in range(500)]
        answers = [table[n][k] if k <= n else 0 for n, k in queries]
        write_case(args.out_dir, 6 + offset, modulus, q, queries, answers)

    modulus = 998244353
    maximum = 9_999_999
    queries = [
        (maximum, 0), (maximum, 1), (maximum, 2), (maximum, 3),
        (maximum, 10), (maximum, maximum - 10),
        (maximum, maximum - 1), (maximum, maximum),
        (maximum - 1, 100), (maximum - 1, maximum - 2),
    ]
    answers = [math.comb(n, min(k, n - k)) % modulus
               for n, k in queries]
    write_case(args.out_dir, 17, modulus, 1, queries, answers)

    queries = [
        (maximum, 0), (maximum, 1), (maximum, 2), (maximum, 3),
        (maximum, 10), (maximum, maximum - 10),
        (maximum, maximum - 1), (maximum, maximum),
        (maximum - 1, 100), (maximum - 1, maximum - 2),
    ]
    answers = [q_lucas_minus_one(n, k, modulus) for n, k in queries]
    write_case(args.out_dir, 18, modulus, modulus - 1, queries, answers)

    # Exercise the uint32_t boundary: adding two residues must not overflow
    # before the reduction modulo p.
    small_case(args.out_dir, 19, 4_294_967_291, 4_294_967_290)


if __name__ == "__main__":
    main()
