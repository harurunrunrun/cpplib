#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
from pathlib import Path


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)

    cases = [
        (1, 12),
        (2, 20),
        (998244353, 40),
        (0xDEADBEEF, 80),
        (0x123456789ABCDEF0, 120),
    ]
    cases.extend(
        (0x9E3779B97F4A7C15 ^ index, 12 + (37 * index) % 109)
        for index in range(32)
    )
    with (args.out_dir / "case_00.in").open(
        "w", encoding="utf-8", newline="\n"
    ) as input_file:
        input_file.write(f"{len(cases)}\n")
        input_file.writelines(f"{seed} {size}\n" for seed, size in cases)
    with (args.out_dir / "case_00.out").open(
        "w", encoding="utf-8", newline="\n"
    ) as output_file:
        output_file.writelines("OK\n" for _ in cases)


if __name__ == "__main__":
    main()
