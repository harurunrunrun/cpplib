#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import math
import random
from pathlib import Path

MOD = 998244353
MAX_N = 1000


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    factorial = [1] * (MAX_N + 1)
    for i in range(1, MAX_N + 1):
        factorial[i] = factorial[i - 1] * i % MOD
    inverse_factorial = [1] * (MAX_N + 1)
    inverse_factorial[MAX_N] = pow(factorial[MAX_N], MOD - 2, MOD)
    for i in range(MAX_N, 0, -1):
        inverse_factorial[i - 1] = inverse_factorial[i] * i % MOD

    bell = [1]
    row = [1]
    for _ in range(60):
        next_row = [row[-1]]
        for value in row:
            next_row.append((next_row[-1] + value) % MOD)
        row = next_row
        bell.append(row[0])

    rng = random.Random(20260713)
    commands = ["FACT 0", "FACT 1000", "IFACT 1000", "INV 1", "COMB 5 2", "PERM 5 6", "HOM 0 0", "HOM 0 7", "BELL 0", "BELL 20"]
    outputs = [
        "1",
        str(factorial[1000]),
        str(inverse_factorial[1000]),
        "1",
        "10",
        "0",
        "1",
        "0",
        "1",
        str(bell[20]),
    ]
    kinds = ["FACT", "IFACT", "INV", "PERM", "COMB", "HOM", "BELL"]

    for _ in range(500):
        kind = rng.choice(kinds)
        if kind == "FACT":
            n = rng.randrange(MAX_N + 1)
            commands.append(f"FACT {n}")
            outputs.append(str(factorial[n]))
        elif kind == "IFACT":
            n = rng.randrange(MAX_N + 1)
            commands.append(f"IFACT {n}")
            outputs.append(str(inverse_factorial[n]))
        elif kind == "INV":
            n = rng.randrange(1, MAX_N + 1)
            commands.append(f"INV {n}")
            outputs.append(str(pow(n, MOD - 2, MOD)))
        elif kind in {"PERM", "COMB"}:
            n = rng.randrange(MAX_N + 1)
            k = rng.randrange(-5, MAX_N + 6)
            commands.append(f"{kind} {n} {k}")
            if k < 0 or k > n:
                outputs.append("0")
            elif kind == "PERM":
                outputs.append(str(factorial[n] * inverse_factorial[n - k] % MOD))
            else:
                outputs.append(str(math.comb(n, k) % MOD))
        elif kind == "HOM":
            if rng.randrange(5) == 0:
                n = 0
                k = rng.randrange(30)
            else:
                n = rng.randrange(1, 501)
                k = rng.randrange(1001 - n)
            commands.append(f"HOM {n} {k}")
            if n == 0:
                outputs.append(str(int(k == 0)))
            else:
                outputs.append(str(math.comb(n + k - 1, k) % MOD))
        else:
            n = rng.randrange(61)
            commands.append(f"BELL {n}")
            outputs.append(str(bell[n]))

    (out_dir / "case_00.in").write_text(
        str(len(commands)) + "\n" + "\n".join(commands) + "\n",
        encoding="utf-8",
    )
    (out_dir / "case_00.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
