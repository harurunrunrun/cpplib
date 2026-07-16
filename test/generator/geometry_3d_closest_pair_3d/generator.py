#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
from pathlib import Path


CASES = (
    (0, 32, 0),
    (1, 64, 0),
    (0x9E3779B97F4A7C15, 96, 0),
    (0xD1B54A32D192ED03, 128, 0),
    (123456789, 0, 200_000),
)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)
    for index, (seed, rounds, performance_size) in enumerate(CASES):
        stem = f"case_{index:02d}"
        (out_dir / f"{stem}.in").write_text(
            f"{seed} {rounds} {performance_size}\n", encoding="ascii"
        )
        (out_dir / f"{stem}.out").write_text("OK\n", encoding="ascii")


if __name__ == "__main__":
    main()
