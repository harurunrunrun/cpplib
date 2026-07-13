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

    for index, seed in enumerate((1, 20260713, 18446744073709551557)):
        stem = f"case_{index:02d}"
        (args.out_dir / f"{stem}.in").write_text(f"{seed}\n", encoding="utf-8")
        (args.out_dir / f"{stem}.out").write_text("OK\n", encoding="utf-8")


if __name__ == "__main__":
    main()
