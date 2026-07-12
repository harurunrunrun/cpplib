#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import math
import random
from pathlib import Path

MOD = 998244353


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(20260713)
    commands = [
        "ADD -1 998244353",
        "SUB 0 1",
        "MUL -3 -7",
        "POW 0 0",
        "INV 1",
        "INV12 5",
        "INC 998244352",
        "DEC 0",
    ]
    outputs = ["998244352", "998244352", "21", "1", "1", "5", "0", "998244352"]

    kinds = ["ADD", "SUB", "MUL", "DIV", "POW", "INV", "INV12", "INC", "DEC"]
    for _ in range(700):
        kind = rng.choice(kinds)
        a = rng.randrange(-10**18, 10**18 + 1)
        if kind in {"ADD", "SUB", "MUL"}:
            b = rng.randrange(-10**18, 10**18 + 1)
            commands.append(f"{kind} {a} {b}")
            if kind == "ADD":
                outputs.append(str((a + b) % MOD))
            elif kind == "SUB":
                outputs.append(str((a - b) % MOD))
            else:
                outputs.append(str((a * b) % MOD))
        elif kind == "DIV":
            b = rng.randrange(-10**18, 10**18 + 1)
            while b % MOD == 0:
                b = rng.randrange(-10**18, 10**18 + 1)
            commands.append(f"DIV {a} {b}")
            outputs.append(str(a % MOD * pow(b, MOD - 2, MOD) % MOD))
        elif kind == "POW":
            exponent = rng.randrange(10**18 + 1)
            commands.append(f"POW {a} {exponent}")
            outputs.append(str(pow(a, exponent, MOD)))
        elif kind == "INV":
            while a % MOD == 0:
                a = rng.randrange(-10**18, 10**18 + 1)
            commands.append(f"INV {a}")
            outputs.append(str(pow(a, MOD - 2, MOD)))
        elif kind == "INV12":
            while math.gcd(a, 12) != 1:
                a = rng.randrange(-10**18, 10**18 + 1)
            commands.append(f"INV12 {a}")
            outputs.append(str(pow(a, -1, 12)))
        elif kind == "INC":
            commands.append(f"INC {a}")
            outputs.append(str((a + 1) % MOD))
        else:
            commands.append(f"DEC {a}")
            outputs.append(str((a - 1) % MOD))

    (out_dir / "case_00.in").write_text(
        str(len(commands)) + "\n" + "\n".join(commands) + "\n",
        encoding="utf-8",
    )
    (out_dir / "case_00.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
