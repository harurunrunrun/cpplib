#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
from pathlib import Path


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    for index, repeat in enumerate([1, 7]):
        (out_dir / f"case_{index:02d}.in").write_text(
            f"{repeat}\n",
            encoding="utf-8",
        )
        (out_dir / f"case_{index:02d}.out").write_text(
            "OK\n",
            encoding="utf-8",
        )


if __name__ == "__main__":
    main()
