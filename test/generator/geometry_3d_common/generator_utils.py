#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
from pathlib import Path


CASES = (
    (0, 32),
    (1, 64),
    (0x9E3779B97F4A7C15, 96),
    (0xD1B54A32D192ED03, 128),
)


def generate() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    for index, (seed, rounds) in enumerate(CASES):
        stem = f"case_{index:02d}"
        (out_dir / f"{stem}.in").write_text(
            f"{seed} {rounds}\n",
            encoding="utf-8",
        )
        (out_dir / f"{stem}.out").write_text("OK\n", encoding="utf-8")
