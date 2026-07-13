#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import math
import random
from pathlib import Path


def brute(a: list[int], b: list[int], use_gcd: bool) -> list[int]:
    n = len(a)
    result = [0] * n
    for i in range(1, n + 1):
        for j in range(1, n + 1):
            index = math.gcd(i, j) if use_gcd else math.lcm(i, j)
            if index <= n:
                result[index - 1] += a[i - 1] * b[j - 1]
    return result


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(20260713)
    for case_id, use_gcd in enumerate((True, False)):
        n = 180
        a = [rng.randrange(-30, 31) for _ in range(n)]
        b = [rng.randrange(-30, 31) for _ in range(n)]
        answer = brute(a, b, use_gcd)
        lines = ["GCD" if use_gcd else "LCM", str(n)]
        input_text = f"{lines[0]} {lines[1]}\n" + " ".join(map(str, a)) + "\n" + " ".join(map(str, b)) + "\n"
        (out_dir / f"case_{case_id:02d}.in").write_text(input_text, encoding="utf-8")
        (out_dir / f"case_{case_id:02d}.out").write_text(
            " ".join(map(str, answer)) + "\n", encoding="utf-8"
        )


if __name__ == "__main__":
    main()
