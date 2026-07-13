#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import math
from pathlib import Path

MOD = 998244353


def bernoulli(maximum: int) -> list[int]:
    answer = [1]
    for index in range(1, maximum + 1):
        total = sum(math.comb(index + 1, previous) * answer[previous]
                    for previous in range(index)) % MOD
        answer.append(-total * pow(index + 1, MOD - 2, MOD) % MOD)
    return answer


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)
    cases = [0, 1, 2, 3, 4, 5, 10, 20, 50, 100, 300]
    (args.out_dir / "case_00.in").write_text(
        str(len(cases)) + "\n" + "".join(f"{n}\n" for n in cases),
        encoding="utf-8",
    )
    (args.out_dir / "case_00.out").write_text(
        "".join(" ".join(map(str, bernoulli(n))) + "\n" for n in cases),
        encoding="utf-8",
    )


if __name__ == "__main__":
    main()
