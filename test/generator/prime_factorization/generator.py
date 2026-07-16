#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import math
import random
from pathlib import Path


def factorize(n: int) -> list[tuple[int, int]]:
    result: list[tuple[int, int]] = []
    divisor = 2
    while divisor * divisor <= n:
        if n % divisor == 0:
            exponent = 0
            while n % divisor == 0:
                n //= divisor
                exponent += 1
            result.append((divisor, exponent))
        divisor = 3 if divisor == 2 else divisor + 2
    if n > 1:
        result.append((n, 1))
    return result


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(20260713)
    values = [1, 2, 60, 99991, 600851475143, 1000000007, 1000003 * 1000033]
    values.extend(rng.randrange(1, 10**9 + 1) for _ in range(260))
    commands: list[str] = []
    outputs: list[str] = []
    for n in values:
        factors = factorize(n)
        for kind in ["PRIME", "FACT", "PHI"]:
            commands.append(f"{kind} {n}")
            if kind == "PRIME":
                outputs.append(str(int(n >= 2 and len(factors) == 1 and factors[0] == (n, 1))))
            elif kind == "FACT":
                flat = [len(factors)]
                for prime, exponent in factors:
                    flat.extend((prime, exponent))
                outputs.append(" ".join(map(str, flat)))
            else:
                phi = n
                for prime, _ in factors:
                    phi = phi // prime * (prime - 1)
                outputs.append(str(phi))

    commands.append("STEP 123456789 18446744073709551555 18446744073709551557")
    outputs.append("15241578750190519")
    near_u64_maximum = 18_446_743_979_220_271_189
    commands.extend([
        f"PRIME {near_u64_maximum}",
        f"FACT {near_u64_maximum}",
        f"PHI {near_u64_maximum}",
    ])
    outputs.extend([
        "0",
        "2 4294967279 1 4294967291 1",
        "18446743970630336620",
    ])

    (out_dir / "case_00.in").write_text(str(len(commands)) + "\n" + "\n".join(commands) + "\n", encoding="utf-8")
    (out_dir / "case_00.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
