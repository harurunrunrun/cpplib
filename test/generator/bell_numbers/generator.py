#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import math
from pathlib import Path

MOD = 998244353


def bell(maximum: int) -> list[int]:
    answer = [1]
    for n in range(1, maximum + 1):
        answer.append(sum(
            math.comb(n - 1, k) * answer[k] for k in range(n)
        ) % MOD)
    return answer


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)
    cases = [0, 1, 2, 3, 4, 7, 16, 31, 64, 100]
    (args.out_dir / "case_00.in").write_text(
        str(len(cases)) + "\n" + "".join(f"{n}\n" for n in cases),
        encoding="utf-8",
    )
    (args.out_dir / "case_00.out").write_text(
        "".join(" ".join(map(str, bell(n))) + "\n" for n in cases),
        encoding="utf-8",
    )


if __name__ == "__main__":
    main()
