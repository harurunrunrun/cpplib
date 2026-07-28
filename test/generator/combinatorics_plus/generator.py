#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import math
import random
from pathlib import Path

MOD = 998244353


def eulerian(order: int) -> list[int]:
    if order == 0:
        return [1]
    row = [1]
    for size in range(2, order + 1):
        nxt = [0] * size
        for descents in range(size):
            if descents < len(row):
                nxt[descents] += (descents + 1) * row[descents]
            if descents:
                nxt[descents] += (size - descents) * row[descents - 1]
            nxt[descents] %= MOD
        row = nxt
    return row


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    parser.add_argument("--seed", type=int, default=20260728)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(args.seed)
    cases: list[tuple[int, tuple[int, ...]]] = []
    primes = [2, 3, 5, 7, 11, 13, 17, 19]
    composites = [1, 4, 6, 8, 9, 12, 25, 27, 60, 97, 100, 343]
    for _ in range(320):
        n = rng.randint(0, 350)
        k = rng.randint(0, 390)
        prime = rng.choice(primes)
        exponent = rng.randint(1, 4)
        modulus = rng.choice(composites)
        cases.append((0, (n, k, prime, exponent, modulus)))
    for _ in range(80):
        n = rng.randrange(10**17, 10**18)
        k = rng.randint(0, 50)
        prime = rng.choice(primes)
        exponent = rng.randint(1, 4)
        modulus = rng.choice(composites)
        cases.append((0, (n, k, prime, exponent, modulus)))
    for maximum_index in range(0, 61):
        cases.append((1, (maximum_index,)))
    for order in range(0, 51):
        cases.append((2, (order,)))
    input_lines = [str(len(cases))]
    output_lines = []
    catalan = [1]
    for index in range(60):
        catalan.append(
            catalan[-1] * (4 * index + 2)
            * pow(index + 2, MOD - 2, MOD) % MOD
        )
    for kind, data in cases:
        input_lines.append(" ".join(map(str, (kind, *data))))
        if kind == 0:
            n, k, prime, exponent, modulus = data
            value = math.comb(n, k) if k <= n else 0
            output_lines.append(
                f"{value % (prime ** exponent)} {value % modulus}"
            )
        elif kind == 1:
            output_lines.append(
                " ".join(map(str, catalan[:data[0] + 1]))
            )
        else:
            output_lines.append(" ".join(map(str, eulerian(data[0]))))
    (args.out_dir / "cases.in").write_text(
        "\n".join(input_lines) + "\n", encoding="utf-8"
    )
    (args.out_dir / "cases.out").write_text(
        "\n".join(output_lines) + "\n", encoding="utf-8"
    )


if __name__ == "__main__":
    main()
