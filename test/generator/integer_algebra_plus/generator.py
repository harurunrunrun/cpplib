#!/usr/bin/env python3

import argparse
from pathlib import Path


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    arguments = parser.parse_args()
    output = Path(arguments.out_dir)
    output.mkdir(parents=True, exist_ok=True)
    for index, seed in enumerate((1, 19, 20260728, 0x5EED1234)):
        (output / f"{index}.in").write_text(f"{seed} 100\n")
        (output / f"{index}.out").write_text("OK\n")


if __name__ == "__main__":
    main()
