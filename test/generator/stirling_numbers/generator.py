#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
from pathlib import Path

MOD = 998244353


def rows(order: int) -> tuple[list[int], list[int]]:
    first = [1]
    second = [1]
    for n in range(1, order + 1):
        next_first = [0] * (n + 1)
        next_second = [0] * (n + 1)
        for k in range(n + 1):
            if k < n:
                next_first[k] -= (n - 1) * first[k]
                next_second[k] += k * second[k]
            if k > 0:
                next_first[k] += first[k - 1]
                next_second[k] += second[k - 1]
            next_first[k] %= MOD
            next_second[k] %= MOD
        first, second = next_first, next_second
    return first, second


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)
    cases = [0, 1, 2, 3, 4, 7, 16, 31, 64, 100]
    outputs = []
    for order in cases:
        first, second = rows(order)
        outputs.append(" ".join(map(str, first)))
        outputs.append(" ".join(map(str, second)))
    (args.out_dir / "case_00.in").write_text(
        str(len(cases)) + "\n" + "".join(f"{n}\n" for n in cases),
        encoding="utf-8",
    )
    (args.out_dir / "case_00.out").write_text(
        "\n".join(outputs) + "\n", encoding="utf-8"
    )


if __name__ == "__main__":
    main()
