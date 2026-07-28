#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path

MOD = 998244353


def convolution_prefix(left: list[int], right: list[int]) -> list[int]:
    return [
        sum(left[index] * right[degree - index]
            for index in range(degree + 1)) % MOD
        for degree in range(len(left))
    ]


def online_prefix(values: list[int], kernel: list[int]) -> list[int]:
    return [
        sum(values[index] * kernel[degree - index]
            for index in range(degree + 1)
            if degree - index < len(kernel)) % MOD
        for degree in range(len(values))
    ]


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    parser.add_argument("--seed", type=int, default=20260728)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(args.seed)
    relaxed: list[tuple[list[int], list[int]]] = [
        ([3], [4]),
        ([1, 2, 3, 4], [5, 6, 7, 8]),
    ]
    online: list[tuple[list[int], list[int]]] = [
        ([1, 2, 3], [4, 5]),
        ([7, 8, 9], []),
    ]
    for _ in range(80):
        count = rng.randint(1, 160)
        relaxed.append((
            [rng.randrange(MOD) for _ in range(count)],
            [rng.randrange(MOD) for _ in range(count)],
        ))
    for _ in range(80):
        count = rng.randint(1, 180)
        online.append((
            [rng.randrange(MOD) for _ in range(count)],
            [rng.randrange(MOD) for _ in range(rng.randint(0, 170))],
        ))

    input_lines = [str(len(relaxed) + len(online))]
    output_lines: list[str] = []
    for left, right in relaxed:
        input_lines.append(f"0 {len(left)}")
        input_lines.extend(f"{a} {b}" for a, b in zip(left, right))
        output_lines.append(" ".join(map(str, convolution_prefix(left, right))))
    for values, kernel in online:
        input_lines.append(f"1 {len(values)}")
        input_lines.append(str(len(kernel)))
        input_lines.append(" ".join(map(str, kernel)))
        input_lines.append(" ".join(map(str, values)))
        output_lines.append(" ".join(map(str, online_prefix(values, kernel))))
    (args.out_dir / "cases.in").write_text(
        "\n".join(input_lines) + "\n", encoding="utf-8"
    )
    (args.out_dir / "cases.out").write_text(
        "\n".join(output_lines) + "\n", encoding="utf-8"
    )


if __name__ == "__main__":
    main()
