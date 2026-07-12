#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def encode(value: str) -> str:
    return value if value else "-"


def lcs(a: str, b: str) -> str:
    dp = [[0] * (len(b) + 1) for _ in range(len(a) + 1)]
    for i, left in enumerate(a):
        for j, right in enumerate(b):
            if left == right:
                dp[i + 1][j + 1] = dp[i][j] + 1
            else:
                dp[i + 1][j + 1] = max(dp[i][j + 1], dp[i + 1][j])
    result: list[str] = []
    i, j = len(a), len(b)
    while i > 0 and j > 0:
        if a[i - 1] == b[j - 1]:
            result.append(a[i - 1])
            i -= 1
            j -= 1
        elif dp[i - 1][j] >= dp[i][j - 1]:
            i -= 1
        else:
            j -= 1
    return "".join(reversed(result))


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(20260713)
    cases = [("", ""), ("abcde", "ace"), ("abc", "def"), ("abracadabra", "avadakedavra")]
    for _ in range(800):
        a = "".join(rng.choice("abcde") for _ in range(rng.randrange(51)))
        b = "".join(rng.choice("abcde") for _ in range(rng.randrange(51)))
        cases.append((a, b))
    lines = [str(len(cases))]
    outputs = []
    for a, b in cases:
        lines.append(f"{encode(a)} {encode(b)}")
        answer = lcs(a, b)
        outputs.append(f"{len(answer)} {encode(answer)}")
    (out_dir / "case_00.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (out_dir / "case_00.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
