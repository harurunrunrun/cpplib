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
        ("1.1e-10", "1e-6"),
        ("1e-6", "1e-3"),
        ("0.5", "0.25"),
        ("10", "1000"),
    ]
    input_path = args.out_dir / "case_00.in"
    output_path = args.out_dir / "case_00.out"
    with input_path.open("w", encoding="utf-8", newline="\n") as input_file:
        input_file.write(f"{len(cases)}\n")
        input_file.writelines(f"{first} {second}\n" for first, second in cases)
    with output_path.open("w", encoding="utf-8", newline="\n") as output_file:
        output_file.writelines("OK\n" for _ in cases)


if __name__ == "__main__":
    main()
