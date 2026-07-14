#!/usr/bin/env python3

import argparse
from pathlib import Path


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)

    modes = ("official", "state", "subword", "stream")
    for index, mode in enumerate(modes):
        stem = f"{index:04d}"
        (args.out_dir / f"{stem}.in").write_text(
            mode + "\n", encoding="utf-8"
        )
        (args.out_dir / f"{stem}.out").write_text("OK\n", encoding="utf-8")


if __name__ == "__main__":
    main()
