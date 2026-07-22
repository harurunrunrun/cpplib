#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
from pathlib import Path


CASES = (
    (0, 8, 32, 128),
    (1, 16, 64, 256),
    (0x9E3779B97F4A7C15, 32, 96, 512),
    (0xD1B54A32D192ED03, 48, 160, 1024),
    (0x94D049BB133111EB, 64, 384, 2048),
)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    output = Path(args.out_dir)
    output.mkdir(parents=True, exist_ok=True)
    for index, values in enumerate(CASES):
        stem = f"case_{index:02d}"
        (output / f"{stem}.in").write_text(
            " ".join(map(str, values)) + "\n", encoding="utf-8"
        )
        (output / f"{stem}.out").write_text("OK\n", encoding="utf-8")


if __name__ == "__main__":
    main()
