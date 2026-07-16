#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import math
import random
from pathlib import Path


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    directory = Path(args.out_dir)
    directory.mkdir(parents=True, exist_ok=True)

    rng = random.Random(2026071618)
    commands: list[str] = []
    output: list[str] = []
    edges = [0, 1, -1, 2, -2, (1 << 63) - 1, -(1 << 63),
             (1 << 1024) - 1, -((1 << 1024) - 1)]
    pairs = [(left, right) for left in edges for right in edges]
    for _ in range(800):
        left = rng.getrandbits(rng.randrange(1, 2049))
        right = rng.getrandbits(rng.randrange(1, 2049))
        if rng.randrange(2):
            left = -left
        if rng.randrange(2):
            right = -right
        common = rng.getrandbits(rng.randrange(1, 257)) | 1
        pairs.append((left * common, right * common))

    for left, right in pairs:
        gcd = math.gcd(left, right)
        operation = rng.randrange(3)
        if operation == 0:
            commands.append(f"GCD {left} {right}")
            output.append(str(gcd))
        elif operation == 1:
            commands.append(f"EXT {left} {right}")
            output.append(str(gcd))
        else:
            commands.append(f"LCM {left} {right}")
            output.append(str(0 if gcd == 0 else abs(left // gcd * right)))

    (directory / "case_00.in").write_text(
        str(len(commands)) + "\n" + "\n".join(commands) + "\n", encoding="utf-8"
    )
    (directory / "case_00.out").write_text(
        "\n".join(output) + "\n", encoding="utf-8"
    )


if __name__ == "__main__":
    main()

