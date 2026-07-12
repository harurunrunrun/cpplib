#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
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
        "POW10 0 0",
        "POW10 2 6",
        "POW12 6 0",
        "POW998 998244353 0",
        "NEST10 2 3 4",
        "NEST998 2 3 4",
        "ADD10 7 8",
        "MUL10 7 8",
    ]
    outputs = ["1", "4", "1", "1", str(pow(2, 3**4, 10)), str(pow(2, 3**4, MOD)), "5", "6"]
    kinds = ["POW10", "POW12", "POW998", "NEST10", "NEST998", "ADD10", "MUL10"]

    for _ in range(650):
        kind = rng.choice(kinds)
        a = rng.randrange(10**18 + 1)
        if kind in {"NEST10", "NEST998"}:
            b = rng.randrange(41)
            c = rng.randrange(9)
            modulus = 10 if kind == "NEST10" else MOD
            commands.append(f"{kind} {a} {b} {c}")
            outputs.append(str(pow(a, b**c, modulus)))
        else:
            b = rng.randrange(10**18 + 1)
            commands.append(f"{kind} {a} {b}")
            if kind == "POW10":
                outputs.append(str(pow(a, b, 10)))
            elif kind == "POW12":
                outputs.append(str(pow(a, b, 12)))
            elif kind == "POW998":
                outputs.append(str(pow(a, b, MOD)))
            elif kind == "ADD10":
                outputs.append(str((a + b) % 10))
            else:
                outputs.append(str((a * b) % 10))

    (out_dir / "case_00.in").write_text(
        str(len(commands)) + "\n" + "\n".join(commands) + "\n",
        encoding="utf-8",
    )
    (out_dir / "case_00.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
