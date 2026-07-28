#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path

MASK64 = (1 << 64) - 1


def nim_product(left: int, right: int, bits: int = 64) -> int:
    if bits == 1:
        return left & right
    half = bits // 2
    mask = (1 << half) - 1
    left_low = left & mask
    left_high = left >> half
    right_low = right & mask
    right_high = right >> half
    high = nim_product(left_high, right_high, half)
    low = nim_product(left_low, right_low, half)
    mixed = nim_product(
        left_low ^ left_high, right_low ^ right_high, half
    )
    twist = nim_product(high, 1 << (half // 2), half)
    return (((mixed ^ low) << half) ^ twist ^ low) & MASK64


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(0x641A)
    cases = [
        (0, 0), (0, MASK64), (1, MASK64), (2, 2), (2, 3),
        (1 << 63, 1 << 63), (MASK64, MASK64),
    ]
    cases.extend((rng.getrandbits(64), rng.getrandbits(64))
                 for _ in range(300))
    (args.out_dir / "case_00.in").write_text(
        str(len(cases)) + "\n"
        + "".join(f"{left} {right}\n" for left, right in cases),
        encoding="utf-8",
    )
    (args.out_dir / "case_00.out").write_text(
        "".join(f"{nim_product(left, right)}\n" for left, right in cases),
        encoding="utf-8",
    )


if __name__ == "__main__":
    main()
