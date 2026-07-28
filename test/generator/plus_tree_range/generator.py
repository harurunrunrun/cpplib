#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
from pathlib import Path


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    arguments = parser.parse_args()
    output = Path(arguments.out_dir)
    output.mkdir(parents=True, exist_ok=True)
    (output / "case_00.in").write_text("tree-range-regression\n", encoding="utf-8")
    (output / "case_00.out").write_text("", encoding="utf-8")


if __name__ == "__main__":
    main()
