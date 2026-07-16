#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
from pathlib import Path


CASES = (
    ("degenerate_and_large", 0, 32),
    ("combination_oracle_a", 1, 64),
    ("combination_oracle_b", 0x9E3779B97F4A7C15, 96),
    ("random_spatial", 0xD1B54A32D192ED03, 128),
    ("different_shuffle_seed", 0x94D049BB133111EB, 96),
    ("facet_boundary_cleanup", 0x243F6A8885A308D3, 8),
)


def generate() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    for name, seed, rounds in CASES:
        (out_dir / f"{name}.in").write_text(
            f"{seed} {rounds}\n",
            encoding="utf-8",
        )
        (out_dir / f"{name}.out").write_text(
            "OK\n",
            encoding="utf-8",
        )


if __name__ == "__main__":
    generate()
