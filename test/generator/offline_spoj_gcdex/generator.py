#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import math
import random
from pathlib import Path


def brute_pairwise_gcd_sum(upper_bound: int) -> int:
    """Quadratic oracle used only for small cases."""
    return sum(
        math.gcd(left, right)
        for right in range(2, upper_bound + 1)
        for left in range(1, right)
    )


def totients_by_linear_sieve(limit: int) -> list[int]:
    """Independent linear-sieve reference for the large cases."""
    phi = [0] * (limit + 1)
    if limit >= 1:
        phi[1] = 1
    composite = bytearray(limit + 1)
    primes: list[int] = []
    for value in range(2, limit + 1):
        if not composite[value]:
            primes.append(value)
            phi[value] = value - 1
        for prime in primes:
            product = value * prime
            if product > limit:
                break
            composite[product] = 1
            if value % prime == 0:
                phi[product] = phi[value] * prime
                break
            phi[product] = phi[value] * (prime - 1)
    return phi


def large_reference(upper_bound: int, phi: list[int]) -> int:
    # For gcd(left, right) = divisor, the reduced pair contributes
    # phi(quotient); summing divisors first yields this independent formula.
    return sum(
        phi[quotient]
        * (upper_bound // quotient)
        * (upper_bound // quotient + 1)
        // 2
        for quotient in range(2, upper_bound + 1)
    )


def write_case(
    out_dir: Path,
    name: str,
    queries: list[int],
    answers: list[int],
) -> None:
    if len(queries) != len(answers):
        raise ValueError("queries and answers must have equal lengths")
    input_text = "\n".join(map(str, [*queries, 0])) + "\n"
    output_text = "".join(f"{answer}\n" for answer in answers)
    (out_dir / f"{name}.in").write_text(input_text, encoding="ascii")
    (out_dir / f"{name}.out").write_text(output_text, encoding="ascii")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    # A leading zero is the input terminator and exercises the empty query list.
    write_case(out_dir, "zero", [], [])

    known = [1, 2, 3, 4, 10, 100]
    write_case(
        out_dir,
        "known",
        known,
        [brute_pairwise_gcd_sum(value) for value in known],
    )

    source = random.Random(0x6CD_E)
    random_queries = [source.randint(1, 160) for _ in range(80)]
    write_case(
        out_dir,
        "small_random",
        random_queries,
        [brute_pairwise_gcd_sum(value) for value in random_queries],
    )

    stress = [200_000, 999_983, 1_000_000]
    phi = totients_by_linear_sieve(max(stress))
    write_case(
        out_dir,
        "stress",
        stress,
        [large_reference(value, phi) for value in stress],
    )


if __name__ == "__main__":
    main()
