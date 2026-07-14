#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import math
import random
from pathlib import Path


def reference_lcm(values: list[int]) -> int:
    result = 1
    for value in values:
        if result == 0 or value == 0:
            result = 0
        else:
            result = abs(result // math.gcd(result, value) * value)
    return result


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(20260714)
    cases = [
        [],
        [1],
        [2, 3, 4],
        [-6, 10, -15],
        [0, 7, 11],
        [6, 10, 15, 21, 35],
        [rng.randrange(-20, 21) for _ in range(80)],
    ]
    input_lines = [str(len(cases))]
    output_lines = []
    for values in cases:
        input_lines.append(str(len(values)))
        input_lines.append(" ".join(map(str, values)))
        output_lines.append(str(reference_lcm(values)))

    (out_dir / "case_00.in").write_text(
        "\n".join(input_lines) + "\n", encoding="utf-8"
    )
    (out_dir / "case_00.out").write_text(
        "\n".join(output_lines) + "\n", encoding="utf-8"
    )


if __name__ == "__main__":
    main()
