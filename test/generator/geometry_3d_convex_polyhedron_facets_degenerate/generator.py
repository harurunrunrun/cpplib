#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
from pathlib import Path


CASES = (
    (
        "valid_triangle",
        "2 3 1\n0 0 0\n2 0 0\n0 2 0\n0 1 2\n",
        "OK 1\n",
    ),
    (
        "collinear_distinct_indices",
        "2 3 1\n0 0 0\n1 0 0\n2 0 0\n0 1 2\n",
        "DOMAIN\n",
    ),
    (
        "duplicate_coordinates",
        "2 3 1\n0 0 0\n1 0 0\n1 0 0\n0 1 2\n",
        "DOMAIN\n",
    ),
    (
        "translated_collinear",
        "2 3 1\n1000000000000000000 3 -7\n"
        "1000000000000000001 5 -4\n"
        "1000000000000000002 7 -1\n0 1 2\n",
        "DOMAIN\n",
    ),
)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    output = Path(args.out_dir)
    output.mkdir(parents=True, exist_ok=True)
    for index, (name, input_text, expected_text) in enumerate(CASES):
        stem = f"case_{index:02d}_{name}"
        (output / f"{stem}.in").write_text(input_text, encoding="utf-8")
        (output / f"{stem}.out").write_text(expected_text, encoding="utf-8")


if __name__ == "__main__":
    main()
