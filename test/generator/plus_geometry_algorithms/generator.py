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
    for index, count in enumerate((1, 3, 20)):
        (args.out_dir / f"case_{index:02d}.in").write_text(
            f"{count}\n", encoding="utf-8"
        )
        (args.out_dir / f"case_{index:02d}.out").write_text(
            "OK\n" * count, encoding="utf-8"
        )


if __name__ == "__main__":
    main()
