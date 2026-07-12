#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import math
import random
from pathlib import Path


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(20260713)
    identities = [
        ("EADD", 0),
        ("EMUL", 1),
        ("EMIN", (1 << 31) - 1),
        ("EMAX", -(1 << 31)),
        ("EGCD", 0),
        ("ELCM", 1),
        ("EXOR", 0),
        ("EAND", -1),
        ("EOR", 0),
    ]
    commands = [name for name, _ in identities]
    outputs = [str(value) for _, value in identities]
    kinds = ["ADD", "MUL", "MIN", "MAX", "GCD", "LCM", "XOR", "AND", "OR"]

    for _ in range(650):
        kind = rng.choice(kinds)
        if kind == "ADD":
            a, b = rng.randrange(-10**12, 10**12), rng.randrange(-10**12, 10**12)
            result = a + b
        elif kind == "MUL":
            a, b = rng.randrange(-10**6, 10**6), rng.randrange(-10**6, 10**6)
            result = a * b
        elif kind == "MIN":
            a, b = rng.randrange(-(1 << 30), 1 << 30), rng.randrange(-(1 << 30), 1 << 30)
            result = min(a, b)
        elif kind == "MAX":
            a, b = rng.randrange(-(1 << 30), 1 << 30), rng.randrange(-(1 << 30), 1 << 30)
            result = max(a, b)
        elif kind == "GCD":
            a, b = rng.randrange(-(1 << 30), 1 << 30), rng.randrange(-(1 << 30), 1 << 30)
            result = math.gcd(a, b)
        elif kind == "LCM":
            a, b = rng.randrange(1, 30000), rng.randrange(1, 30000)
            result = math.lcm(a, b)
        else:
            a, b = rng.randrange(1 << 30), rng.randrange(1 << 30)
            result = {"XOR": a ^ b, "AND": a & b, "OR": a | b}[kind]
        commands.append(f"{kind} {a} {b}")
        outputs.append(str(result))

    (out_dir / "case_00.in").write_text(
        str(len(commands)) + "\n" + "\n".join(commands) + "\n",
        encoding="utf-8",
    )
    (out_dir / "case_00.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
