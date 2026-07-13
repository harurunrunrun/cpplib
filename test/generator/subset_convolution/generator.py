#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path

MOD = 998244353


def naive(left: list[int], right: list[int]) -> list[int]:
    answer = [0] * len(left)
    for subset in range(len(left)):
        part = subset
        while True:
            answer[subset] += left[part] * right[subset ^ part]
            if part == 0:
                break
            part = (part - 1) & subset
        answer[subset] %= MOD
    return answer


def write_case(out_dir: Path, index: int,
               cases: list[tuple[int, list[int], list[int], list[int]]]) -> None:
    parts = [str(len(cases))]
    outputs = []
    for bits, left, right, answer in cases:
        parts.extend([str(bits), " ".join(map(str, left)), " ".join(map(str, right))])
        outputs.append(" ".join(map(str, answer)))
    (out_dir / f"case_{index:02d}.in").write_text(
        "\n".join(parts) + "\n", encoding="utf-8"
    )
    (out_dir / f"case_{index:02d}.out").write_text(
        "\n".join(outputs) + "\n", encoding="utf-8"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(2026071312)
    cases = []
    for bits in range(0, 11):
        for _ in range(8):
            size = 1 << bits
            left = [rng.randrange(MOD) for _ in range(size)]
            right = [rng.randrange(MOD) for _ in range(size)]
            cases.append((bits, left, right, naive(left, right)))
    write_case(args.out_dir, 0, cases)

    bits = 20
    size = 1 << bits
    ones = [1] * size
    answer = [1 << index.bit_count() for index in range(size)]
    write_case(args.out_dir, 1, [(bits, ones, ones, answer)])


if __name__ == "__main__":
    main()
