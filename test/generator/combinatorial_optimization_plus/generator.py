#!/usr/bin/env python3

import argparse
from pathlib import Path


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    arguments = parser.parse_args()
    output = Path(arguments.out_dir)
    output.mkdir(parents=True, exist_ok=True)
    cases = (
        (1, 60),
        (31, 60),
        (998244353, 60),
        (0xA7B01D, 60),
        (0, 0),
    )
    for index, (seed, rounds) in enumerate(cases):
        (output / f"{index}.in").write_text(f"{seed} {rounds}\n")
        (output / f"{index}.out").write_text("OK\n")


if __name__ == "__main__":
    main()
