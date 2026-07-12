#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def sieve_prefix(limit: int) -> list[int]:
    is_prime = bytearray(b"\x01") * (limit + 1)
    is_prime[0:2] = b"\x00\x00"
    for prime in range(2, int(limit**0.5) + 1):
        if is_prime[prime]:
            start = prime * prime
            is_prime[start : limit + 1 : prime] = b"\x00" * (((limit - start) // prime) + 1)
    prefix = [0] * (limit + 1)
    for value in range(1, limit + 1):
        prefix[value] = prefix[value - 1] + is_prime[value]
    return prefix


def write_case(out_dir: Path, name: str, queries: list[int], answers: list[int]) -> None:
    (out_dir / f"{name}.in").write_text(
        str(len(queries)) + "\n" + "\n".join(map(str, queries)) + "\n",
        encoding="utf-8",
    )
    (out_dir / f"{name}.out").write_text(
        "\n".join(map(str, answers)) + "\n",
        encoding="utf-8",
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    limit = 2_000_000
    prefix = sieve_prefix(limit)
    rng = random.Random(20260713)
    small_queries = [0, 1, 2, 3, 10, limit]
    small_queries.extend(rng.randrange(limit + 1) for _ in range(100))
    write_case(out_dir, "case_00", small_queries, [prefix[value] for value in small_queries])

    # Values from the standard table of pi(10^k).
    large_queries = [10**8, 10**10, 10**12]
    large_answers = [5_761_455, 455_052_511, 37_607_912_018]
    write_case(out_dir, "case_01", large_queries, large_answers)


if __name__ == "__main__":
    main()
