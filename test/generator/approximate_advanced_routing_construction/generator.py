#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
from pathlib import Path


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    arguments = parser.parse_args()
    output = Path(arguments.out_dir)
    output.mkdir(parents=True, exist_ok=True)
    cases = (
        (1, 12),
        (2026072901, 16),
        (0xC0DEC0DE, 18),
        (0xFFFFFFFFFFFFFFFF, 10),
    )
    for index, (seed, rounds) in enumerate(cases):
        (output / f"{index}.in").write_text(
            f"{seed} {rounds}\n", encoding="utf-8"
        )
        (output / f"{index}.out").write_text("OK\n", encoding="utf-8")


if __name__ == "__main__":
    main()
