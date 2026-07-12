#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


MOD = 998244353


def write_case(out_dir: Path, idx: int, a: int, b: int, e: int) -> None:
    name = f"case_{idx:02d}"
    (out_dir / f"{name}.in").write_text(f"{a} {b} {e}\n", encoding="utf-8")
    out = [(a + b) % MOD, (a - b) % MOD, (a * b) % MOD, pow(a % MOD, e, MOD)]
    (out_dir / f"{name}.out").write_text(" ".join(map(str, out)) + "\n", encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()

    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    write_case(out_dir, 0, 2, 3, 10)
    rng = random.Random(20260917)
    for i in range(1, 6):
        write_case(out_dir, i, rng.randrange(-10**12, 10**12), rng.randrange(-10**12, 10**12), rng.randrange(10**6))


if __name__ == "__main__":
    main()
