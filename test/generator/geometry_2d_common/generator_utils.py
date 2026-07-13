#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
from pathlib import Path


def generate(cases: list[tuple[str, str]]) -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    for index, (input_data, output_data) in enumerate(cases):
        stem = f"case_{index:02d}"
        (out_dir / f"{stem}.in").write_text(input_data, encoding="utf-8")
        (out_dir / f"{stem}.out").write_text(output_data, encoding="utf-8")
