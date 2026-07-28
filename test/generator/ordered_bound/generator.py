#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
from pathlib import Path


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", type=Path, required=True)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)
    (args.out_dir / "fixed.in").write_text("", encoding="utf-8")
    (args.out_dir / "fixed.out").write_text("OK\n", encoding="utf-8")


if __name__ == "__main__":
    main()
