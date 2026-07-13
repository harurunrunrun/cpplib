#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)

    random_source = random.Random(2026071308)
    cases = ["0", "1", "01010101", "0" * 24, "1" * 24]
    for length in (4, 8, 12, 16, 20, 24):
        cases.append("".join(random_source.choice("01") for _ in range(length)))
    (args.out_dir / "cases.in").write_text(
        f"{len(cases)}\n" + "\n".join(cases) + "\n",
        encoding="utf-8",
    )
    (args.out_dir / "cases.out").write_text("OK\n", encoding="utf-8")


if __name__ == "__main__":
    main()
