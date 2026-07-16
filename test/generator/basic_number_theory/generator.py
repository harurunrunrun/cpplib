#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import math
import random
from pathlib import Path


def divisors(n: int) -> list[int]:
    n = abs(n)
    small: list[int] = []
    large: list[int] = []
    d = 1
    while d * d <= n:
        if n % d == 0:
            small.append(d)
            if d * d != n:
                large.append(n // d)
        d += 1
    return small + large[::-1]


def quotient_ranges(n: int) -> list[tuple[int, int, int]]:
    result: list[tuple[int, int, int]] = []
    left = 1
    while left <= n:
        value = n // left
        right = n // value + 1
        result.append((left, right, value))
        left = right
    return result


def to_digits(n: int, base: int) -> list[int]:
    if n == 0:
        return [0]
    result: list[int] = []
    while n:
        result.append(n % base)
        n //= base
    return result[::-1]


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(20260713)
    commands: list[str] = []
    outputs: list[str] = []

    def add(command: str, output: str | int) -> None:
        commands.append(command)
        outputs.append(str(output))

    add("SQLE 3037000499 9223372036854775807", 1)
    add("SQLE 3037000500 9223372036854775807", 0)
    add("GCD -18 24", 6)
    add("LCM 0 10", 0)
    add("EXT 30 18", "6 6")
    add("POW -2 5 13", 7)
    add("INV 3 11", 4)
    add("ISQRT 9223372036854775807", math.isqrt((1 << 63) - 1))
    add("DIVISORS -36", "9 1 2 3 4 6 9 12 18 36")
    add("QUOTIENT 0", "0")
    add("TODIG 0 2", "1 0")
    add("FROM 16 2 15 15", 255)

    kinds = ["GCD", "LCM", "EXT", "POW", "INV", "ISQRT", "DIVISORS", "QUOTIENT", "TODIG", "FROM"]
    for _ in range(280):
        kind = rng.choice(kinds)
        if kind in {"GCD", "LCM", "EXT"}:
            a = rng.randrange(-10**6, 10**6 + 1)
            b = rng.randrange(-10**6, 10**6 + 1)
            g = math.gcd(a, b)
            if kind == "GCD":
                add(f"GCD {a} {b}", g)
            elif kind == "LCM":
                add(f"LCM {a} {b}", math.lcm(a, b))
            else:
                add(f"EXT {a} {b}", f"{g} {g}")
        elif kind == "POW":
            x = rng.randrange(-10**18, 10**18 + 1)
            exponent = rng.randrange(10**18 + 1)
            mod = rng.randrange(1, 10**12)
            add(f"POW {x} {exponent} {mod}", pow(x, exponent, mod))
        elif kind == "INV":
            mod = rng.randrange(2, 10**9)
            a = rng.randrange(-10**9, 10**9)
            while math.gcd(a, mod) != 1:
                a = rng.randrange(-10**9, 10**9)
            add(f"INV {a} {mod}", pow(a, -1, mod))
        elif kind == "ISQRT":
            n = rng.randrange(1 << 63)
            add(f"ISQRT {n}", math.isqrt(n))
        elif kind == "DIVISORS":
            n = rng.randrange(1, 10**9)
            if rng.randrange(2):
                n = -n
            values = divisors(n)
            add(f"DIVISORS {n}", " ".join(map(str, [len(values), *values])))
        elif kind == "QUOTIENT":
            n = rng.randrange(200001)
            ranges = quotient_ranges(n)
            flat: list[int] = [len(ranges)]
            for left, right, value in ranges:
                flat.extend((left, right, value))
            add(f"QUOTIENT {n}", " ".join(map(str, flat)))
        elif kind == "TODIG":
            n = rng.randrange(1 << 64)
            base = rng.randrange(2, 37)
            digits = to_digits(n, base)
            add(f"TODIG {n} {base}", " ".join(map(str, [len(digits), *digits])))
        else:
            n = rng.randrange(1 << 64)
            base = rng.randrange(2, 37)
            digits = to_digits(n, base)
            add(
                "FROM " + " ".join(map(str, [base, len(digits), *digits])),
                n,
            )

    (out_dir / "case_00.in").write_text(
        str(len(commands)) + "\n" + "\n".join(commands) + "\n",
        encoding="utf-8",
    )
    (out_dir / "case_00.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
