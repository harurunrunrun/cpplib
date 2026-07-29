#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    arguments = parser.parse_args()
    arguments.out_dir.mkdir(parents=True, exist_ok=True)
    random_engine = random.Random(0xE701A12)
    batches = [[1, 2, 3], [4, 7]]
    for _ in range(4):
        batches.append([
            random_engine.randint(1, 9) for _ in range(8)
        ])
    expected = " ".join(["0"] * 15)
    for index, dimensions in enumerate(batches):
        (arguments.out_dir / f"{index}.in").write_text(
            f"{len(dimensions)}\n"
            + "".join(f"{dimension}\n" for dimension in dimensions),
            encoding="utf-8",
        )
        (arguments.out_dir / f"{index}.out").write_text(
            "".join(f"{expected}\n" for _ in dimensions),
            encoding="utf-8",
        )


if __name__ == "__main__":
    main()
