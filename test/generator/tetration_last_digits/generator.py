#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def tetration(base: int, height: int) -> int:
    if height == 0:
        return 1
    if base == 0:
        return 0 if height % 2 == 1 else 1
    if base == 1:
        return 1
    return pow(base, tetration(base, height - 1))


def solve(base: int, height: int, digit_count: int) -> str:
    exact = tetration(base, height)
    modulus = 10**digit_count
    value = exact % modulus
    truncated = exact >= modulus
    rendered = (
        f"...{value:0{digit_count}d}"
        if truncated
        else str(value)
    )
    return f"{value} {digit_count} {int(truncated)} {rendered}"


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[tuple[int, int, int]] = [
        (0, 0, 1),
        (0, 1, 19),
        (0, 2, 19),
        (0, (1 << 64) - 1, 9),
        (1, (1 << 64) - 1, 19),
        (10, 1, 1),
        (10, 1, 2),
        (999_999_999_999_999_999, 1, 19),
        (2, 4, 1),
        (2, 4, 19),
        (5, 3, 9),
    ]
    source = random.Random(0x7E7A710)
    for _ in range(300):
        family = source.randrange(3)
        if family == 0:
            base = source.randrange(0, 51)
            height = source.randrange(0, 3)
        elif family == 1:
            base = source.randrange(0, 6)
            height = 3
        else:
            base = source.randrange(0, 3)
            height = 4
        digit_count = source.randrange(1, 20)
        cases.append((base, height, digit_count))

    input_lines = [str(len(cases))]
    input_lines.extend(
        f"{base} {height} {digit_count}"
        for base, height, digit_count in cases
    )
    output_lines = [
        solve(base, height, digit_count)
        for base, height, digit_count in cases
    ]
    (out_dir / "case_00.in").write_text(
        "\n".join(input_lines) + "\n",
        encoding="ascii",
    )
    (out_dir / "case_00.out").write_text(
        "\n".join(output_lines) + "\n",
        encoding="ascii",
    )


if __name__ == "__main__":
    main()
