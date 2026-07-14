#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def write_case(out_dir: Path, index: int, number: int) -> None:
    values = sorted({number // divisor for divisor in range(1, number + 1)})
    (out_dir / f"case_{index:02d}.in").write_text(
        f"{number}\n", encoding="utf-8"
    )
    (out_dir / f"case_{index:02d}.out").write_text(
        f"{len(values)}\n" + " ".join(map(str, values)) + "\n",
        encoding="utf-8",
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    arguments = parser.parse_args()
    out_dir = Path(arguments.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    fixed = [0, 1, 2, 10, 36, 999, 10000]
    for index, number in enumerate(fixed):
        write_case(out_dir, index, number)

    rng = random.Random(20260714)
    for index in range(len(fixed), len(fixed) + 16):
        write_case(out_dir, index, rng.randrange(0, 100001))


if __name__ == "__main__":
    main()
