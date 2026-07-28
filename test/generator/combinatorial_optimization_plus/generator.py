#!/usr/bin/env python3

import argparse
from pathlib import Path


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    arguments = parser.parse_args()
    output = Path(arguments.out_dir)
    output.mkdir(parents=True, exist_ok=True)
    for index, seed in enumerate((1, 31, 998244353, 0xA7B01D)):
        (output / f"{index}.in").write_text(f"{seed} 60\n")
        (output / f"{index}.out").write_text("OK\n")


if __name__ == "__main__":
    main()
