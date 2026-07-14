#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def is_prime_naive(value: int) -> bool:
    if value < 2:
        return False
    divisor = 2
    while divisor * divisor <= value:
        if value % divisor == 0:
            return False
        divisor += 1
    return True


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(20260714)
    cases = [
        [],
        [-7, -2, -1, 0, 1, 2, 3, 4, 5, 9, 97],
        list(range(201)),
        [99991, 99989, 99990, 100003, 100005],
        [rng.randrange(-100, 200001) for _ in range(400)],
    ]
    input_lines = [str(len(cases))]
    output_lines = []
    for values in cases:
        input_lines.append(str(len(values)))
        input_lines.append(" ".join(map(str, values)))
        output_lines.append(str(sum(is_prime_naive(value) for value in values)))

    (out_dir / "case_00.in").write_text(
        "\n".join(input_lines) + "\n", encoding="utf-8"
    )
    (out_dir / "case_00.out").write_text(
        "\n".join(output_lines) + "\n", encoding="utf-8"
    )


if __name__ == "__main__":
    main()
