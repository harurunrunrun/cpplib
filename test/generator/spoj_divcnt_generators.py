#!/usr/bin/env python3

import argparse
import math
import random
from pathlib import Path


def write_case(out_dir, name, input_text, output_text):
    (out_dir / f"{name}.in").write_text(input_text)
    (out_dir / f"{name}.out").write_text(output_text)


def divisor_summatory_oracle(n):
    total = 0
    left = 1
    while left <= n:
        quotient = n // left
        right = n // quotient
        total += quotient * (right - left + 1)
        left = right + 1
    return total


def generate_divcnt1(out_dir):
    rng = random.Random(193)
    values = [1, 2, 3, 10, 100, 99991]
    values += [rng.randrange(1, 200_000) for _ in range(36)]
    values += [10**8, 10**10, 10**12]
    answers = [divisor_summatory_oracle(n) for n in values]
    write_case(
        out_dir,
        "mixed",
        f"{len(values)}\n" + "\n".join(map(str, values)) + "\n",
        "\n".join(map(str, answers)) + "\n",
    )
    write_case(
        out_dir,
        "maximum",
        "1\n9223372036854775807\n",
        "404193099932803760669\n",
    )


def square_divisor_prefix_oracle(limit):
    smallest = list(range(limit + 1))
    for prime in range(2, math.isqrt(limit) + 1):
        if smallest[prime] != prime:
            continue
        for multiple in range(prime * prime, limit + 1, prime):
            if smallest[multiple] == multiple:
                smallest[multiple] = prime
    prefix = [0] * (limit + 1)
    for value in range(1, limit + 1):
        remaining = value
        count = 1
        while remaining > 1:
            prime = smallest[remaining]
            exponent = 0
            while remaining % prime == 0:
                remaining //= prime
                exponent += 1
            count *= 2 * exponent + 1
        prefix[value] = prefix[value - 1] + count
    return prefix


def generate_divcnt2(out_dir):
    rng = random.Random(194)
    values = [1, 2, 3, 10, 100, 99991, 200000]
    values += [rng.randrange(1, 200_001) for _ in range(36)]
    prefix = square_divisor_prefix_oracle(max(values))
    answers = [prefix[n] for n in values]
    write_case(
        out_dir,
        "mixed",
        f"{len(values)}\n" + "\n".join(map(str, values)) + "\n",
        "\n".join(map(str, answers)) + "\n",
    )
    write_case(
        out_dir,
        "maximum",
        "1\n1000000000000\n",
        "263628273676370\n",
    )


GENERATORS = {
    "divcnt1": generate_divcnt1,
    "divcnt2": generate_divcnt2,
}


def main(code=None, argv=None):
    parser = argparse.ArgumentParser()
    if code is None:
        parser.add_argument("code", choices=sorted(GENERATORS))
    parser.add_argument("--out-dir", type=Path, required=True)
    parser.add_argument("--seed", type=int, default=0)
    args = parser.parse_args(argv)
    args.out_dir.mkdir(parents=True, exist_ok=True)
    GENERATORS[code if code is not None else args.code](args.out_dir)


if __name__ == "__main__":
    main()
