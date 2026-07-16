#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def solve_brute(text: bytes) -> int:
    size = len(text)
    if size == 0:
        return 0
    dp = [0] * size
    for left in range(size - 1, -1, -1):
        diagonal = 0
        for right in range(left + 1, size):
            old = dp[right]
            if text[left] == text[right]:
                dp[right] = diagonal
            else:
                dp[right] = 1 + min(dp[right], dp[right - 1])
            diagonal = old
    return dp[-1]


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    texts = [
        b"",
        b"a",
        b"abba",
        b"fft",
        b"abc",
        b"Aa",
        b"racecar",
        b"google",
        b"aabb",
        b"abcdefghijklmnopqrstuvwxyz",
        bytes(range(256)),
        bytes(range(256)) + bytes(range(255, -1, -1)),
    ]
    source = random.Random(1021097)
    for size in (2, 62, 63, 64, 65, 66, 127, 128, 129):
        texts.append(bytes(source.randrange(256) for _ in range(size)))
    for _ in range(500):
        texts.append(bytes(
            source.randrange(256) for _ in range(source.randrange(97))
        ))

    expected = [solve_brute(text) for text in texts]
    two_blocks = bytes(10_000) + bytes([1]) * 10_000
    texts.append(two_blocks)
    expected.append(10_000)
    half = bytes(index % 251 for index in range(10_000))
    texts.append(half + half[::-1])
    expected.append(0)

    (out_dir / "case_00.in").write_text(
        "\n".join([
            str(len(texts)),
            *(text.hex() if text else "-" for text in texts),
        ]) + "\n",
        encoding="ascii",
    )
    (out_dir / "case_00.out").write_text(
        "\n".join(map(str, expected)) + "\n",
        encoding="ascii",
    )


if __name__ == "__main__":
    main()
