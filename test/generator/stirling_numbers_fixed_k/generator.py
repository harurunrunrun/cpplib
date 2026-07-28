#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
from pathlib import Path

MOD = 998244353


def table(maximum: int) -> tuple[list[list[int]], list[list[int]]]:
    first = [[0] * (maximum + 1) for _ in range(maximum + 1)]
    second = [[0] * (maximum + 1) for _ in range(maximum + 1)]
    first[0][0] = second[0][0] = 1
    for n in range(1, maximum + 1):
        for k in range(1, n + 1):
            first[n][k] = (
                first[n - 1][k - 1] - (n - 1) * first[n - 1][k]
            ) % MOD
            second[n][k] = (
                second[n - 1][k - 1] + k * second[n - 1][k]
            ) % MOD
    return first, second


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)
    cases = [
        (0, 0), (1, 0), (1, 1), (8, 1), (8, 4), (8, 8),
        (31, 7), (64, 32), (100, 0), (100, 99),
    ]
    maximum = max(n for n, _ in cases)
    first, second = table(maximum)
    outputs = []
    for n, k in cases:
        outputs.append(" ".join(str(first[i][k]) for i in range(n + 1)))
        outputs.append(" ".join(str(second[i][k]) for i in range(n + 1)))
    (args.out_dir / "case_00.in").write_text(
        str(len(cases)) + "\n"
        + "".join(f"{n} {k}\n" for n, k in cases),
        encoding="utf-8",
    )
    (args.out_dir / "case_00.out").write_text(
        "\n".join(outputs) + "\n", encoding="utf-8"
    )


if __name__ == "__main__":
    main()
