#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def encode(value: bytes) -> str:
    return value.hex() if value else "-"


def lcs_length(a: bytes, b: bytes) -> int:
    dp = [0] * (len(b) + 1)
    for left in a:
        diagonal = 0
        for j, right in enumerate(b):
            old = dp[j + 1]
            if left == right:
                dp[j + 1] = diagonal + 1
            else:
                dp[j + 1] = max(dp[j + 1], dp[j])
            diagonal = old
    return dp[-1]


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(20260713)
    cases: list[tuple[bytes, bytes, int]] = []

    def add(a: bytes, b: bytes) -> None:
        cases.append((a, b, lcs_length(a, b)))

    add(b"", b"")
    add(b"abcde", b"ace")
    add(b"abc", b"def")
    add(b"abracadabra", b"avadakedavra")
    all_bytes = bytes(range(256))
    add(all_bytes, all_bytes)
    add(all_bytes, all_bytes[::-1])
    for size in (1, 2, 62, 63, 64, 65, 66, 127, 128, 129):
        add(
            bytes(rng.randrange(256) for _ in range(size)),
            bytes(rng.randrange(256) for _ in range(size + 3)),
        )
    for _ in range(600):
        add(
            bytes(rng.randrange(256) for _ in range(rng.randrange(97))),
            bytes(rng.randrange(256) for _ in range(rng.randrange(97))),
        )
    large = bytes(index % 251 for index in range(20_000))
    cases.append((large, large, len(large)))

    lines = [str(len(cases))]
    outputs: list[str] = []
    for a, b, expected in cases:
        lines.append(f"{encode(a)} {encode(b)}")
        outputs.append(str(expected))
    (out_dir / "case_00.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (out_dir / "case_00.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
