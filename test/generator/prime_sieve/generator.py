#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def is_prime_sqrt(n: int) -> bool:
    if n <= 1:
        return False
    divisor = 2
    while divisor * divisor <= n:
        if n % divisor == 0:
            return False
        divisor += 1
    return True


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    limit = 150000
    minimum = list(range(limit + 1))
    minimum[0] = 0
    if limit >= 1:
        minimum[1] = 1
    for p in range(2, int(limit**0.5) + 1):
        if minimum[p] == p:
            for value in range(p * p, limit + 1, p):
                if minimum[value] == value:
                    minimum[value] = p
    primes = [n for n in range(2, limit + 1) if minimum[n] == n]

    def factors(n: int) -> list[tuple[int, int]]:
        result: list[tuple[int, int]] = []
        while n > 1:
            prime = minimum[n]
            exponent = 0
            while n % prime == 0:
                n //= prime
                exponent += 1
            result.append((prime, exponent))
        return result

    rng = random.Random(20260713)
    commands = [
        "BOUND 3037000499 9223372036854775807",
        "BOUND 3037000500 9223372036854775807",
        "SQRT -1",
        "SQRT 99991",
        "SQRT 9223372036854775807",
        "IS 0",
        f"IS {limit}",
        "MIN 1",
        "KTH 0",
        "COUNT",
        "FACT 1",
    ]
    outputs = [
        "1", "0", "0", "1", "0", "0",
        str(int(minimum[limit] == limit)), "1", "2", str(len(primes)), "0",
    ]
    kinds = ["SQRT", "IS", "MIN", "KTH", "COUNT", "FACT"]
    for _ in range(1600):
        kind = rng.choice(kinds)
        if kind == "COUNT":
            commands.append("COUNT")
            outputs.append(str(len(primes)))
        elif kind == "KTH":
            k = rng.randrange(len(primes))
            commands.append(f"KTH {k}")
            outputs.append(str(primes[k]))
        elif kind == "SQRT":
            n = rng.randrange(-100, 10**8)
            commands.append(f"SQRT {n}")
            outputs.append(str(int(is_prime_sqrt(n))))
        elif kind == "IS":
            n = rng.randrange(limit + 1)
            commands.append(f"IS {n}")
            outputs.append(str(int(n >= 2 and minimum[n] == n)))
        elif kind == "MIN":
            n = rng.randrange(limit + 1)
            commands.append(f"MIN {n}")
            outputs.append(str(minimum[n]))
        else:
            n = rng.randrange(1, limit + 1)
            result = factors(n)
            flat = [len(result)]
            for prime, exponent in result:
                flat.extend((prime, exponent))
            commands.append(f"FACT {n}")
            outputs.append(" ".join(map(str, flat)))

    (out_dir / "case_00.in").write_text(f"{limit} {len(commands)}\n" + "\n".join(commands) + "\n", encoding="utf-8")
    (out_dir / "case_00.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
