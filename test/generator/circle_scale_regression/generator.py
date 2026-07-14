#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def write_case(output_directory: Path, index: int, scales: list[float]) -> None:
    with (output_directory / f"case_{index:02d}.in").open(
        "w", encoding="utf-8", newline="\n"
    ) as input_file, (output_directory / f"case_{index:02d}.out").open(
        "w", encoding="utf-8", newline="\n"
    ) as output_file:
        input_file.write(f"{len(scales)}\n")
        input_file.writelines(f"{scale:.18g}\n" for scale in scales)
        output_file.writelines("OK\n" for _ in scales)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    arguments = parser.parse_args()
    arguments.out_dir.mkdir(parents=True, exist_ok=True)

    write_case(arguments.out_dir, 0, [1.0, 1e-5, 1e5])

    source = random.Random(2026071510)
    scales = [10.0 ** source.uniform(-5.0, 5.0) for _ in range(1000)]
    write_case(arguments.out_dir, 1, scales)


if __name__ == "__main__":
    main()
