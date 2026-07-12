#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def discrete_log_naive(a: int, target: int, mod: int) -> int:
    if mod == 1:
        return 0
    a %= mod
    target %= mod
    current = 1 % mod
    seen: set[int] = set()
    exponent = 0
    while current not in seen:
        if current == target:
            return exponent
        seen.add(current)
        current = current * a % mod
        exponent += 1
    return -1


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    cases = [(2, 1, 13), (2, 8, 13), (2, 8, 12), (0, 0, 17), (0, 2, 17), (7, 7, 1)]
    rng = random.Random(20260713)
    for _ in range(1400):
        mod = rng.randrange(1, 2001)
        a = rng.randrange(-5000, 5001)
        target = rng.randrange(-5000, 5001)
        cases.append((a, target, mod))

    inputs = [str(len(cases))]
    inputs.extend(f"{a} {target} {mod}" for a, target, mod in cases)
    outputs = [str(discrete_log_naive(a, target, mod)) for a, target, mod in cases]
    (out_dir / "case_00.in").write_text("\n".join(inputs) + "\n", encoding="utf-8")
    (out_dir / "case_00.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
