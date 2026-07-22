#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
from pathlib import Path


CASES = (
    ("random_00", 0, 32),
    ("random_01", 1, 64),
    ("random_02", 0x9E3779B97F4A7C15, 96),
    # Every invocation also exercises dense coplanar facets, duplicate points,
    # equal-x layers, a moment curve, and a subnormal-height bipyramid.
    ("differential_512", 123456789, 512),
    # rounds == 1 additionally constructs 50,000 interior points.
    ("performance_50000", 0x243F6A8885A308D3, 1),
)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    for stem, seed, rounds in CASES:
        (out_dir / f"{stem}.in").write_text(
            f"{seed} {rounds}\n",
            encoding="utf-8",
        )
        (out_dir / f"{stem}.out").write_text("OK\n", encoding="utf-8")
